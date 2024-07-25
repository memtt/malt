/// <reference types="node" />
import { Readable } from 'stream';
declare enum Types {
    Array = 0,
    Object = 1,
    ReadableString = 2,
    ReadableObject = 3,
    Primitive = 4,
    Promise = 5
}
interface IStackItem {
    key?: string;
    index?: number;
    type: Types;
    value: any;
    parent?: IStackItem;
    first: boolean;
    unread?: string[] | number;
    isEmpty?: boolean;
    arrayLength?: number;
    readCount?: number;
    end?: boolean;
    addSeparatorAfterEnd?: boolean;
}
declare class JsonStreamStringify extends Readable {
    private cycle;
    private visited;
    private stack;
    private replacerFunction?;
    private replacerArray?;
    private gap?;
    private depth;
    private error;
    private pushCalled;
    private end;
    private isReading;
    private readMore;
    constructor(value: any, replacer?: Function | any[], spaces?: number | string, cycle?: boolean);
    private cycler;
    private addToStack;
    private removeFromStack;
    private _push;
    private processReadableObject;
    private processObject;
    private processArray;
    processPrimitive(current: IStackItem): void;
    private processReadableString;
    private processPromise;
    private processStackTopItem;
    private __read;
    _read(size: number): void;
    path(): (string | number)[];
}
export default JsonStreamStringify;
