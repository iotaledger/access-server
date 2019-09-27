export interface ControllerInterface {
    /**
     * Gets last published messages.
     * 
     * @param deviceId ID of device.
     */
    getLast(deviceId: string): Promise<any>
}