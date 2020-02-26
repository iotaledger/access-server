import * as utils from '../utils/utils';
import { mandatoryEnvMissing } from '../utils/utils';
import * as Iota from '@iota/core';
import Controller from './controller';
import * as Converter from '@iota/converter';
import * as iotaUtils from '../utils/iota_utils';
import uuid from 'uuid';
const grpc = require('grpc');
const protoLoader = require('@grpc/proto-loader');

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
const DEFAULT_SECURITY_LEVEL = 2;
const DEFAULT_MAX_MIOTA = 2779530283277761;

export class IotaController implements Controller {

  private masterId?: string;
  private decimals: number;
  private hub: any;
  private iota: Iota.API;

  /**
   * Constructor of Iota Controller.
   * 
   * @param iriUrl URL of IRI node, eg. 'http://127.0.0.1:14265'.
   * 
   * @param hubUrl URL of HUB, eg. '127.0.0.1:50051'.
   * 
   * @param decimals  Optional variable. If omitted, default value is 1.
   * It represents number of decimal points used for human readable
   * representation of tokens.
   * 
   * For example if decimals is equal to 3 and user sends someone 27 tokens,
   * internally transaction of 27 000 MIOTAs will be made. It's purpose is to
   * simplifying transactions of large number of tokens.
   * 
   * @param masterId ID of master user in IOTA HUB from who's payments are being
   * made. It can be regularly created account if Token Store is connected to
   * IOTA main Tangle, or it can be created using build in tool for initializing
   * IOTA HUB when using private Tangle.
   */
  constructor(
    iriUrl: string,
    hubUrl: string,
    decimals: number = DEFAULT_DECIMALS,
    masterId?: string
  ) {
    this.masterId = masterId;
    this.decimals = decimals;
    this.hub = new hubProto.Hub(hubUrl, grpc.credentials.createInsecure());
    this.iota = Iota.composeAPI({ provider: iriUrl });
  }

  public setMasterId(masterId: string) {
    this.masterId = masterId;
  }

  /**
   * Initializes [[IotaController]] from process environment.
   * 
   * @throws Error with explanation if some of mandatory process environment
   * variables is missing.
   * 
   * @returns Newly created [[IotaController]].
   */
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

  /** @inheritdoc */
  public async createAccount(): Promise<string> {
    try {
      const userId = uuid.v4();
      return new Promise((resolve, reject) => {
        this.hub.createUser({ userId: userId }, (err: any, response: any) => {
          if (err)
            reject(err);
          else
            resolve(userId);
        });
      });
    } catch (err) {
      return Promise.reject(err)
    }
  }

  /** @inheritdoc */
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

  /** @inheritdoc */
  public send(from: string, to: string, amount: number): Promise<void> {
    return new Promise(async (resolve, reject) => {
      let payoutAddress;
      try {
        payoutAddress = await this.getNewDepositAddress(to);
      } catch (err) {
        reject(err);
        return;
      }

      // Converts provided amount of tokens in human representation with
      // decimals to actual amount of MIOTAs
      const convertedAmount = parseInt(utils.toTokenWithoutDecimals(
        amount,
        this.decimals
      ));

      // Create request for withdraw
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

  /** @inheritdoc */
  public fund(receiver: string, amount: number): Promise<void> {
    if (this.masterId === undefined)
      return Promise.reject('Master ID not provided, unable to fund')
    else
      return this.send(this.masterId, receiver, amount);
  }

  /**
   * Creates Master user on HUB and transfers provided amount of MIOTAs
   * to it.
   * 
   * @param seed Seed of account on tangle containing MIOTAs.
   * @param security Level of security for addresses for provided seed
   * (1, 2 or 3).
   * 
   * @param amount Amount of MIOTAs to be transferred to HUB's master user.
   * Default amount is 2779530283277761 (max number of MIOTAs).
   * 
   * @returns Promise containing ID of newly created master user.
   */
  public async createMasterUserAndFundIt(
    seed: string,
    security: number = DEFAULT_SECURITY_LEVEL,
    amount: number = DEFAULT_MAX_MIOTA
  ): Promise<string> {
    return new Promise(async (resolve, reject) => {
      try {
        // Check if balance for seed is already transferred to HUB
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
          // Account is already emptied out
          reject('account already emptied out');
        } else {
          // Create new master account
          const masterUser = await this.createAccount();
          const masterDepositAddress = await this.getNewDepositAddress(masterUser);

          const tag = '';

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
          this.masterId = masterUser;
          resolve(masterUser);
        }
      } catch (err) {
        reject(err);
      }
    });
  }

  /**
   * Gets new deposit address from HUB for user with provided ID.
   * 
   * @param userId ID of user.
   * 
   * @returns Promise containing new deposit address.
   */
  private getNewDepositAddress(userId: string): Promise<string> {
    return new Promise((resolve, reject) => {
      this.hub.getDepositAddress(
        { userId: userId, includeChecksum: false },
        (err: any, response: any) => {
          if (err)
            reject(err);
          else
            resolve(response.address);
        });
    });
  };
}
