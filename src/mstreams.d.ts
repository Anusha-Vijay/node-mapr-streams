/*
 * we can use https://github.com/Microsoft/dts-gen,
 * to generate this if it gets more complicated.
 *
 */
declare namespace mapr {
    class mstreams {
        constructor()
        create(clientType:string)
    }

    class mproducer extends mstreams {
        constructor();
        get(): mproducer;
        produce(topic: string, partition: number,
                message: string, key?: string,
                opaque?: any,
                cb?:(error?: string, value?: string) => void): any;
        poll(interval: number, cb?:(nummsgs?: number) => void): any;
    }

    class mconsumer extends mstreams {
        constructor();
        get(): mconsumer;
        consume(topic: string, partition: number, timeout:number, group: string,
                cb?:(error?: string, value?: object) => void): any;
    }
}