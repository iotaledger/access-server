export interface Controller {
    createAccount(): Promise<string>
    getBalance(address: string): Promise<string>
    send(from: string, to: string, amount: number): Promise<void>
    fund(receiver: string, amount: number): Promise<void>
}