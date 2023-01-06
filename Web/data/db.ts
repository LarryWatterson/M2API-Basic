const mysql = require('mysql2');
const config = require('../common/config');

class DataBase {
    connection : any;

    constructor() {
        this.connection = null;
    }

    async Connect() : Promise<void> {
        this.connection = mysql.createConnection(config.DB_CONNECTION_INFO);
        this.connection.connect((err: any) => {
            if (err) {
                console.log(`Database connection error: ${err}`);
                return;
            }
            console.log('Connection established');
        });
    }

    async Execute(query: string, params: any[]) : Promise<any> {
        try {
            const [row, ] = await this.connection.promise().execute(query, params);
            return row;
        }
        catch (err) {
            console.log(err);
        }
    }
}

export = new DataBase();