const { SocketClientTCP } = require('netlinkwrapper');
const Config  = require('../common/config');

class GameConnector {
    socket: any;

    constructor() {
        this.socket = null;
    }

    async Connect() {
        this.socket = new SocketClientTCP(Config.SERVER_CONNECTION_INFO.PORT, Config.SERVER_CONNECTION_INFO.IP);
        this.socket.receive();
    }

    async Send(data: any, msg=null) {
        const message = msg == null ? `\x40${data}\x0A` : `\x40${data} ${msg}\x0A`;
        this.socket.send(message);
        const received = this.socket.receive();
        if (received) {
            console.log("Recv: " +  received.toString());
        }
        else {
            console.log('No data received');
        }
    }

    async Close() {
        this.socket.disconnect();
    }
}

export = new GameConnector;