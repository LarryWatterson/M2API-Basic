const db = require('./data/db');
const conn = require('./data/server');
const conf = require('./common/config');
const app = require('./data/web_listener');
const fs = require('fs');

const testSocketConnection = async () => {
    conn.Connect().catch((err: any) => { console.log(err); });
    conn.Send('DC', 'lwt').catch((err: any) => { console.log(err); });
    conn.Close().catch((err: any) => { console.log(err); });
}

const testDBConnection = async(level: number) => {
    db.Connect();
    const result = db.Execute(`SELECT * FROM ${conf.PLAYER_DATABASE}.player WHERE level > ?`, [level]);
    result.then((row: any) => {
        var arr: any[] = [];
        for (const item of row) {
            let data = {
                id: item.id,
                name: item.name,
            }
            arr.push(data);
        }
        console.log(arr);
    }).catch((err: any) => { console.log(err); });
}
testDBConnection(95);
testSocketConnection();