import * as utils from '../utils/utils';
import { mandatoryEnvMissing } from '../utils/utils';
import { ethers } from 'ethers';
import * as Iota from '@iota/core';
import { Controller } from './controller';
import * as Converter from '@iota/converter';
const grpc = require('grpc');
const protoLoader = require('@grpc/proto-loader');
import * as iotaUtils from '../utils/iota_utils';

const PROTO_PATH = __dirname + '../../../protos/hub.proto';

var packageDefinition = protoLoader.loadSync(
  PROTO_PATH,
  {
    keepCase: true,
    longs: String,
    enums: String,
    defaults: true,
    oneofs: true
  }
);

const hubProto = grpc.loadPackageDefinition(packageDefinition).hub.rpc;

const DEFAULT_DECIMALS = 1;

export class IotaController implements Controller {

  private masterId?: string;
  private decimals: number;
  private hub: any;
  private iota: Iota.API;

  /**
   * Constructor of Iota Controller
   * 
   * @param iriUrl URL of IRI node, eg. 'http://127.0.0.1:14265'
   * @param hubUrl URL of HUB, eg. '127.0.0.1:50051'
   * @param decimals  Optional variable. If omitted, default value is 1.
   * It represents number of decimal points used for human readable representation of tokens.
   * For example if DECIMALS=3 and you send someone 27 tokens, internally transaction of 27 000 tokens will be made.
   * This has purpose of simplifying transactions of large number of tokens with low value.
   * @param masterId ID of master user in IOTA HUB from who's account payments are being made.
   * It can be regularly created account if Token Store is connected to IOTA main Tangle,
   * or it can be created using build in tool for initializing IOTA HUB when using private Tangle.
   */
  constructor(iriUrl: string, hubUrl: string, decimals: number, masterId?: string) {
    this.masterId = masterId;
    this.decimals = decimals;
    this.hub = new hubProto.Hub(hubUrl, grpc.credentials.createInsecure());
    this.iota = Iota.composeAPI({ provider: iriUrl });
  }

  public setMasterId(masterId: string) {
    this.masterId = masterId;
  }

  public static initialize(): IotaController {
    const iriHost = process.env.IRI_HOST;
    if (iriHost === undefined) {
      throw mandatoryEnvMissing('IRI_HOST');
    }

    const iriPort = process.env.IRI_PORT;
    if (iriPort === undefined) {
      throw mandatoryEnvMissing('IRI_PORT');
    }

    const hubHost = process.env.HUB_HOST;
    if (hubHost === undefined) {
      throw mandatoryEnvMissing('HUB_HOST');
    }

    const hubPort = process.env.HUB_PORT;
    if (hubPort === undefined) {
      throw mandatoryEnvMissing('HUB_PORT');
    }

    const decimals = +(process.env.DECIMALS || DEFAULT_DECIMALS);

    const iriUrl = 'http://' + iriHost + ':' + iriPort;
    const hubUrl = hubHost + ':' + hubPort;

    return new IotaController(iriUrl, hubUrl, decimals);
  }

  public async createAccount(): Promise<string> {
    try {
      // create ethereum address and use it as userId
      const address = await ethers.Wallet.createRandom().getAddress();
      return new Promise((resolve, reject) => {
        this.hub.createUser({ userId: address }, (err: any, response: any) => {
          if (err)
            reject(err);
          else
            resolve(address);
        });
      });
    } catch (err) {
      return Promise.reject(err)
    }
  }

  public getBalance(address: string): Promise<string> {
    return new Promise((resolve, reject) => {
      this.hub.getBalance({ userId: address }, (err: any, response: any) => {
        if (err)
          reject(err);
        else
          resolve(utils.toTokenWithDecimals(response.available, this.decimals));
      });
    });
  }

  public send(from: string, to: string, amount: number): Promise<void> {
    return new Promise(async (resolve, reject) => {
      let payoutAddress;
      try {
        payoutAddress = await this.getNewDepositAddress(to);
      } catch (err) {
        reject(err);
        return;
      }

      const convertedAmount = parseInt(utils.toTokenWithoutDecimals(amount, this.decimals));

      // create request for withdraw
      const userWithdrawRequest = {
        userId: from,
        payoutAddress: payoutAddress,
        amount: convertedAmount,
        tag: Converter.asciiToTrytes('transfer'),
        validateChecksum: false
      };

      this.hub.userWithdraw(userWithdrawRequest, (err: any, response: any) => {
        if (err) {
          reject(err);
        } else {
          resolve();
        }
      });
    });
  }

  public fund(receiver: string, amount: number): Promise<void> {
    if (this.masterId === undefined)
      return Promise.reject('Master ID not provided, unable to fund')
    else
      return this.send(this.masterId, receiver, amount);
  }

  /**
   * 
   * @param seed SEED of account on tangle containing tokens
   * @param security Level of security for addresses for provided SEED (1, 2 or 3)
   * @param amount Amount of IOTA tokens to be transferred to HUB's master account
   */
  public async transferToHub(seed: string, security: number = 2, amount: number = 2779530283277761) {
    return new Promise(async (resolve, reject) => {
      try {
        // check is balance for seed already transferred to HUB
        const seedAddresses = await this.iota.getNewAddress(seed, {
          index: 0,
          security: security,
          checksum: false,
          returnAll: true,
          total: 1
        }) as readonly string[];

        const balance = await this.iota.getBalances(seedAddresses, 10);
        const isEmpty = balance.balances[0] === 0;

        if (isEmpty) {
          // account already emptied out
          reject('account already emptied out');
        } else {
          // create new master account
          const masterAccount = await this.createAccount();
          const masterDepositAddress = await this.getNewDepositAddress(masterAccount);

          const tag = '';//Converter.asciiToTrytes('initial funding');

          const transfers: Iota.Transfer[] = [
            {
              value: amount,
              address: masterDepositAddress,
              message: tag,
              tag: tag
            }
          ];

          const trytes = await this.iota.prepareTransfers(seed, transfers);
          const bundle = await this.iota.sendTrytes(trytes, 3, 9);

          await iotaUtils.confirmTransaction(this.iota, bundle[0].hash);
          this.masterId = masterAccount;
          resolve(masterAccount);
        }
      } catch (err) {
        reject(err);
      }
    });
  }

  private getNewDepositAddress(userId: string): Promise<string> {
    return new Promise((resolve, reject) => {
      this.hub.getDepositAddress({ userId: userId, includeChecksum: false }, (err: any, response: any) => {
        if (err)
          reject(err);
        else
          resolve(response.address);
      });
    });
  };
}