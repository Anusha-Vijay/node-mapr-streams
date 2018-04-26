const str2ab = require('string-to-arraybuffer');
const mapr = require('bindings')('mstreams');

const topic = "/shayanstream1:test123";
const partition = 0;
const timeout  = 1000;
const group = "grp2";

/**
 * Producer callback
 * @param error
 * @param value
 */
function getP(error, value) {
    if(error != null) {
        console.log('err: ' + error);
        throw new Error(error);
    } else {
        console.log(value);
    }
}

/**
 * Consumer callback
 * @param error
 * @param value
 */
function getC(error, value) {
    if(error != null) {
        console.log('err: ' + error);
        throw new Error(error);
    } else {
        console.log('Consumed ' + value['key'] + " value: " + value['payload']);
    }
}

/**
 * Produce
 */
function produce() {
    /**
     * Producer
     * @type {mapr.mproducer}
     */
    const handle1 = new mapr.mproducer();
    const producer = handle1.get();
    let msgIdx;
    for(msgIdx = 0; msgIdx < 10; msgIdx++) {

        const opaque = str2ab("opaque" + msgIdx);
        const msgkey = "key-" + msgIdx;
        const msgval = "msg-" + msgIdx;
        const type = producer.produce(
            topic, partition,
            msgval, msgkey, opaque,
            getP);
    }
    producer.poll(5000);
}

/**
 * Consume
 */
function consume() {
    /**
     * Consumer
     * @type {mapr.mconsumer}
     */
    const handle2 = new mapr.mconsumer();
    const consumer = handle2.get();
    consumer.consume(topic, partition, timeout, group, getC);
}
const main = function main() {
    produce();
    consume();
};

module.exports = {
    main: main
};

main();