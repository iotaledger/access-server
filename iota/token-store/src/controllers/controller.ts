/**
 * Controller for managing users and tokens.
 */
export default interface Controller {
  /**
   * Creates new account.
   * 
   * @returns Promise containing ID of newly created account.
   */
  createAccount(): Promise<string>

  /**
   * Gets balance of user with provided ID.
   * 
   * @param userId Id of user.
   * 
   * @returns Promise containing balance.
   */
  getBalance(userId: string): Promise<string>

  /**
   * Sends specified amount of tokens from one account to another.
   * 
   * @param from ID of user from which tokens are sent.
   * @param to ID of user to which tokens are sent.
   * @param amount Amount of tokens to be sent.
   */
  send(from: string, to: string, amount: number): Promise<void>

  /**
   * Funds user from master user's account with specified ID with given
   * amount.
   * 
   * @param receiver ID of user that receives tokens.
   * @param amount Amount of tokens to be transferred.
   */
  fund(receiver: string, amount: number): Promise<void>
}
