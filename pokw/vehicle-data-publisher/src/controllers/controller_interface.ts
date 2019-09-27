export interface ControllerInterface {
    publish(data: string, deviceId: string): Promise<void>
}