const str2ab = require('string-to-arraybuffer');
var mapr = require("mapr-streams-node");
var fetch = require('node-fetch');

const topic = "/shayanstream1:test123";
const partition = 0;
const timeout  = 1000;
const group = "grp3";

var appRouter = function (app) {

  app.get("/", function (req, res) {
      res.status(200).send({ message: 'MStreams restful API' });
  });

  app.get("/opensource", function (req, res) {
      fetch('https://hn.algolia.com/api/v1/search?query=hawking&page=0&hitsPerPage=100')
          .then(res => res.json())
          .then(data => {
              var newData = [];
              let msgIdx;
              var obj = data['hits'];
              for(msgIdx = 0; msgIdx < obj.length; msgIdx++) {
                  newData.push({"title": obj[msgIdx]['title'], "author": obj[msgIdx]['author']});
              }
              return newData;
          })
          .then(newData => {
            console.log(newData);
            return newData;
          })
          .then(newData => {
              const handle1 = new mapr.mproducer();
              const producer = handle1.get();
              let msgIdx;
              for(msgIdx = 0; msgIdx < newData.length; msgIdx++) {
                  var data = newData[msgIdx];
                  var opaque = str2ab("opaque" + msgIdx);
                  console.log("producing ... ");
                  producer.produce(topic, partition, JSON.stringify(data), "msgkey-" + msgIdx, opaque,
                      function getP(error, value) {
                        if(error != null) {
                          console.log('err: ' + error);
                          throw new Error(error);
                        } else {
                          console.log(value);
                        }
                  });
              }
              producer.poll(5000);
              res.status(200).send(newData);
          })
          .catch(e => {
            res.status(500).send("Failed!");
          });
  });

  app.get("/consume", function (req, res) {
      var consumed_data = [];
      const handle2 = new mapr.mconsumer();
      const consumer = handle2.get();
      consumer.consume(topic, partition, timeout, group,
          function getC(error, value) {
            if(error != null) {
                console.log('err: ' + error);
                throw new Error(error);
            } else {
                console.log('Consumed ' + value['key'] + " value: " + value['payload']);
                consumed_data.push(value);
            }
      });
      res.setHeader("Access-Control-Allow-Origin", "*");
      res.status(200).send(consumed_data);
  });
};

module.exports = appRouter;