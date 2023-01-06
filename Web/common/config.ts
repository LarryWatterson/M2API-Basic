const INFO = {
    SERVER_INFO : {
        IP : '1.1.1.1',
        DEV_IP : '1.1.1.2',
    },
};

type configType = {
    SERVER_CONNECTION_INFO : {
        IP : string,
        PORT : number,
    },

    DB_CONNECTION_INFO : {
        host : string,
        user : string,
        password : string,
        // database : string,
    },

    PLAYER_DATABASE : string,
    ACCOUNT_DATABASE : string,
};

const config : configType  = {
    SERVER_CONNECTION_INFO : {
        IP : INFO.SERVER_INFO.DEV_IP,
        PORT : 13900,
    },

    DB_CONNECTION_INFO : {
        host : INFO.SERVER_INFO.IP,
        user : 'root',
        password : 'dev',
        // database : 'srv1_player'
    },

    PLAYER_DATABASE : 'srv1_player',
    ACCOUNT_DATABASE : 'srv1_account',
};

export = config;