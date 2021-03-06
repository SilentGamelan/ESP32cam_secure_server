const path = require('path');
const express = require('express');
const WebSocket = require('ws');
const fs = require('fs');
const https = require('https');

const HTTPS_PORT = 443;

var privateKey = fs.readFileSync("server.key", "utf8");
var certificate = fs.readFileSync("server.cert", "utf8");
var credentials = {key: privateKey, cert: certificate};

const app = express();

const httpsServer = https.createServer(credentials, app);
const wsServer = new WebSocket.Server({server: httpsServer});


let connectedClients = [];

wsServer.on("connection", (ws, req)=> {
    console.log("Connected via websockets");

    // !!FIXME - this section doesn't make sense to me, what is it supposed to be reporting?
    //         - the index of the client according to the connectedClients list? 
    ws.on("message", (data) => {
        if(data.indexOf("WEB_CLIENT") !== -1) {
            connectedClients.push(ws);
            console.log("New WEB_CLIENT added: " + data.indexOf("WEB_CLIENT") + "/" + connectedClients.length);
            // data only contains "WEB_CLIENT"
            // console.log('\n', data);
            // Maybe there is some kind of UID in the websocket object that should be referred to?
            //console.log(ws);            
            return; 
        }

        connectedClients.forEach((ws, i) => {
            if(connectedClients[i] == ws && ws.readState == ws.OPEN) {
                ws.send(data);
            } else {
                connectedClients.splice(i, 1);
            }
        });
    });

    ws.on("error", (error) => {
        console.error("Websocket Error - ", error);
    });
});

app.use(express.static("."));
app.get("/", (req, res)=> { 
    res.redirect("/client");
});
app.get("/client", (req, res) => res.sendFile(path.resolve(__dirname, "./client.html")));
httpsServer.listen(HTTPS_PORT, () => console.log(`HTTPS server listening at ${httpsServer.address().address}:${HTTPS_PORT}`));

