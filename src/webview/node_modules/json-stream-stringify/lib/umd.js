(function (global, factory) {
  typeof exports === 'object' && typeof module !== 'undefined' ? module.exports = factory(require('stream')) :
  typeof define === 'function' && define.amd ? define(['stream'], factory) :
  (global = global || self, global.jsonStreamStringify = factory(global.stream));
}(this, (function (stream) { 'use strict';

  function _typeof(obj) {
    if (typeof Symbol === "function" && typeof Symbol.iterator === "symbol") {
      _typeof = function (obj) {
        return typeof obj;
      };
    } else {
      _typeof = function (obj) {
        return obj && typeof Symbol === "function" && obj.constructor === Symbol && obj !== Symbol.prototype ? "symbol" : typeof obj;
      };
    }

    return _typeof(obj);
  }

  function _classCallCheck(instance, Constructor) {
    if (!(instance instanceof Constructor)) {
      throw new TypeError("Cannot call a class as a function");
    }
  }

  function _defineProperties(target, props) {
    for (var i = 0; i < props.length; i++) {
      var descriptor = props[i];
      descriptor.enumerable = descriptor.enumerable || false;
      descriptor.configurable = true;
      if ("value" in descriptor) descriptor.writable = true;
      Object.defineProperty(target, descriptor.key, descriptor);
    }
  }

  function _createClass(Constructor, protoProps, staticProps) {
    if (protoProps) _defineProperties(Constructor.prototype, protoProps);
    if (staticProps) _defineProperties(Constructor, staticProps);
    return Constructor;
  }

  function _inherits(subClass, superClass) {
    if (typeof superClass !== "function" && superClass !== null) {
      throw new TypeError("Super expression must either be null or a function");
    }

    subClass.prototype = Object.create(superClass && superClass.prototype, {
      constructor: {
        value: subClass,
        writable: true,
        configurable: true
      }
    });
    if (superClass) _setPrototypeOf(subClass, superClass);
  }

  function _getPrototypeOf(o) {
    _getPrototypeOf = Object.setPrototypeOf ? Object.getPrototypeOf : function _getPrototypeOf(o) {
      return o.__proto__ || Object.getPrototypeOf(o);
    };
    return _getPrototypeOf(o);
  }

  function _setPrototypeOf(o, p) {
    _setPrototypeOf = Object.setPrototypeOf || function _setPrototypeOf(o, p) {
      o.__proto__ = p;
      return o;
    };

    return _setPrototypeOf(o, p);
  }

  function _assertThisInitialized(self) {
    if (self === void 0) {
      throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
    }

    return self;
  }

  function _possibleConstructorReturn(self, call) {
    if (call && (typeof call === "object" || typeof call === "function")) {
      return call;
    }

    return _assertThisInitialized(self);
  }

  function _slicedToArray(arr, i) {
    return _arrayWithHoles(arr) || _iterableToArrayLimit(arr, i) || _nonIterableRest();
  }

  function _arrayWithHoles(arr) {
    if (Array.isArray(arr)) return arr;
  }

  function _iterableToArrayLimit(arr, i) {
    if (!(Symbol.iterator in Object(arr) || Object.prototype.toString.call(arr) === "[object Arguments]")) {
      return;
    }

    var _arr = [];
    var _n = true;
    var _d = false;
    var _e = undefined;

    try {
      for (var _i = arr[Symbol.iterator](), _s; !(_n = (_s = _i.next()).done); _n = true) {
        _arr.push(_s.value);

        if (i && _arr.length === i) break;
      }
    } catch (err) {
      _d = true;
      _e = err;
    } finally {
      try {
        if (!_n && _i["return"] != null) _i["return"]();
      } finally {
        if (_d) throw _e;
      }
    }

    return _arr;
  }

  function _nonIterableRest() {
    throw new TypeError("Invalid attempt to destructure non-iterable instance");
  }

  var rxEscapable = /[\\"\u0000-\u001f\u007f-\u009f\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g; // table of character substitutions

  var meta = {
    '\b': '\\b',
    '\t': '\\t',
    '\n': '\\n',
    '\f': '\\f',
    '\r': '\\r',
    '"': '\\"',
    '\\': '\\\\'
  };

  function isReadableStream(value) {
    return typeof value.read === 'function' && typeof value.pause === 'function' && typeof value.resume === 'function' && typeof value.pipe === 'function' && typeof value.once === 'function' && typeof value.removeListener === 'function';
  }

  function getType(value) {
    if (!value) return Types.Primitive;
    if (typeof value.then === 'function') return Types.Promise;
    if (isReadableStream(value)) return value._readableState.objectMode ? Types.ReadableObject : Types.ReadableString;
    if (Array.isArray(value)) return Types.Array;
    if (_typeof(value) === 'object' || value instanceof Object) return Types.Object;
    return Types.Primitive;
  }

  var Types;

  (function (Types) {
    Types[Types["Array"] = 0] = "Array";
    Types[Types["Object"] = 1] = "Object";
    Types[Types["ReadableString"] = 2] = "ReadableString";
    Types[Types["ReadableObject"] = 3] = "ReadableObject";
    Types[Types["Primitive"] = 4] = "Primitive";
    Types[Types["Promise"] = 5] = "Promise";
  })(Types || (Types = {}));

  var stackItemOpen = [];
  stackItemOpen[Types.Array] = '[';
  stackItemOpen[Types.Object] = '{';
  stackItemOpen[Types.ReadableString] = '"';
  stackItemOpen[Types.ReadableObject] = '[';
  var stackItemEnd = [];
  stackItemEnd[Types.Array] = ']';
  stackItemEnd[Types.Object] = '}';
  stackItemEnd[Types.ReadableString] = '"';
  stackItemEnd[Types.ReadableObject] = ']';
  var processFunctionLookupTable = [];

  for (var _i = 0, _Object$entries = Object.entries(Types); _i < _Object$entries.length; _i++) {
    var _Object$entries$_i = _slicedToArray(_Object$entries[_i], 2),
        key = _Object$entries$_i[0],
        val = _Object$entries$_i[1];

    if (typeof val === 'number') processFunctionLookupTable[val] = "process".concat(key);
  }

  function escapeString(string) {
    // Modified code, original code by Douglas Crockford
    // Original: https://github.com/douglascrockford/JSON-js/blob/master/json2.js
    // If the string contains no control characters, no quote characters, and no
    // backslash characters, then we can safely slap some quotes around it.
    // Otherwise we must also replace the offending characters with safe escape
    // sequences.
    return string.replace(rxEscapable, function (a) {
      var c = meta[a];
      return typeof c === 'string' ? c : "\\u".concat(a.charCodeAt(0).toString(16).padStart(4, '0'));
    });
  }

  function quoteString(string) {
    return "\"".concat(escapeString(string), "\"");
  }

  function readAsPromised(stream, size) {
    var value = stream.read(size);

    if (value === null) {
      return new Promise(function (resolve, reject) {
        var endListener = function endListener() {
          return resolve(null);
        };

        stream.once('end', endListener);
        stream.once('error', reject);
        stream.once('readable', function () {
          stream.removeListener('end', endListener);
          stream.removeListener('error', reject);
          resolve(stream.read());
        });
      });
    }

    return Promise.resolve(value);
  }

  function recursiveResolve(promise) {
    return promise.then(function (res) {
      return getType(res) === Types.Promise ? recursiveResolve(res) : res;
    });
  }

  var JsonStreamStringify =
  /*#__PURE__*/
  function (_Readable) {
    _inherits(JsonStreamStringify, _Readable);

    function JsonStreamStringify(value, replacer, spaces) {
      var _this;

      var cycle = arguments.length > 3 && arguments[3] !== undefined ? arguments[3] : false;

      _classCallCheck(this, JsonStreamStringify);

      _this = _possibleConstructorReturn(this, _getPrototypeOf(JsonStreamStringify).call(this, {
        encoding: 'utf8'
      }));
      _this.cycle = cycle;
      _this.stack = [];
      _this.depth = 0;
      _this.pushCalled = false;
      _this.end = false;
      _this.isReading = false;
      _this.readMore = false;

      var spaceType = _typeof(spaces);

      if (spaceType === 'string' || spaceType === 'number') {
        _this.gap = Number.isFinite(spaces) ? ' '.repeat(spaces) : spaces;
      }

      Object.assign(_assertThisInitialized(_this), {
        visited: cycle ? new WeakMap() : new WeakSet(),
        replacerFunction: replacer instanceof Function && replacer,
        replacerArray: Array.isArray(replacer) && replacer
      });
      if (replacer instanceof Function) _this.replacerFunction = replacer;
      if (Array.isArray(replacer)) _this.replacerArray = replacer;

      _this.addToStack(value);

      return _this;
    }

    _createClass(JsonStreamStringify, [{
      key: "cycler",
      value: function cycler(key, value) {
        var existingPath = this.visited.get(value);

        if (existingPath) {
          return {
            $ref: existingPath
          };
        }

        var path = this.path();
        if (key !== undefined) path.push(key);
        path = path.map(function (v) {
          return "[".concat(Number.isInteger(v) ? v : quoteString(v), "]");
        });
        this.visited.set(value, path.length ? "$".concat(path.join('')) : '$');
        return value;
      }
    }, {
      key: "addToStack",
      value: function addToStack(value, key, index, parent) {
        var _this2 = this;

        var _a, _b;

        var realValue = value;

        if (this.replacerFunction) {
          realValue = this.replacerFunction(key || index, realValue, this);
        } // ORDER?


        if (realValue && realValue.toJSON instanceof Function) {
          realValue = realValue.toJSON();
        }

        if (realValue instanceof Function || _typeof(value) === 'symbol') {
          realValue = undefined;
        }

        if (key !== undefined && this.replacerArray) {
          if (!this.replacerArray.includes(key)) {
            realValue = undefined;
          }
        }

        var type = getType(realValue);

        if ((parent && parent.type === Types.Array ? true : realValue !== undefined) && type !== Types.Promise) {
          if (parent && !parent.first) {
            this._push(',');
          }

          if (parent) parent.first = false;
        }

        if (realValue !== undefined && type !== Types.Promise && key) {
          if (this.gap) {
            this._push("\n".concat(this.gap.repeat(this.depth), "\"").concat(escapeString(key), "\": "));
          } else {
            this._push("\"".concat(escapeString(key), "\":"));
          }
        }

        if (type !== Types.Primitive) {
          if (this.cycle) {
            // run cycler
            realValue = this.cycler(key || index, realValue);
            type = getType(realValue);
          } else {
            // check for circular structure
            if (this.visited.has(realValue)) {
              throw Object.assign(new Error('Converting circular structure to JSON'), {
                realValue: realValue,
                key: key || index
              });
            }

            this.visited.add(realValue);
          }
        }

        if (!key && index > -1 && this.depth && this.gap) this._push("\n".concat(this.gap.repeat(this.depth)));
        var open = stackItemOpen[type];
        if (open) this._push(open);
        var obj = {
          key: key,
          index: index,
          type: type,
          parent: parent,
          value: realValue,
          first: true
        };

        if (type === Types.Object) {
          this.depth += 1;
          obj.unread = Object.keys(realValue);
          obj.isEmpty = !obj.unread.length;
        } else if (type === Types.Array) {
          this.depth += 1;
          obj.unread = realValue.length;
          obj.arrayLength = obj.unread;
          obj.isEmpty = !obj.unread;
        } else if (type === Types.ReadableString || type === Types.ReadableObject) {
          this.depth += 1;

          if (realValue.readableEnded || ((_a = realValue._readableState) === null || _a === void 0 ? void 0 : _a.endEmitted)) {
            this.emit('error', new Error('Readable Stream has ended before it was serialized. All stream data have been lost'), realValue, key || index);
          } else if (realValue.readableFlowing || ((_b = realValue._readableState) === null || _b === void 0 ? void 0 : _b.flowing)) {
            realValue.pause();
            this.emit('error', new Error('Readable Stream is in flowing mode, data may have been lost. Trying to pause stream.'), realValue, key || index);
          }

          obj.readCount = 0;
          realValue.once('end', function () {
            obj.end = true;

            _this2.__read();
          });
          realValue.once('error', function (err) {
            _this2.error = true;

            _this2.emit('error', err);
          });
        }

        this.stack.unshift(obj);
        return obj;
      }
    }, {
      key: "removeFromStack",
      value: function removeFromStack(item) {
        var type = item.type;
        var isObject = type === Types.Object || type === Types.Array || type === Types.ReadableString || type === Types.ReadableObject;

        if (type !== Types.Primitive) {
          if (!this.cycle) {
            this.visited["delete"](item.value);
          }

          if (isObject) {
            this.depth -= 1;
          }
        }

        var end = stackItemEnd[type];
        if (isObject && !item.isEmpty && this.gap) this._push("\n".concat(this.gap.repeat(this.depth)));
        if (end) this._push(end);
        var stackIndex = this.stack.indexOf(item);
        this.stack.splice(stackIndex, 1);
      } // tslint:disable-next-line:function-name

    }, {
      key: "_push",
      value: function _push(data) {
        this.pushCalled = true;
        this.push(data);
      }
    }, {
      key: "processReadableObject",
      value: function processReadableObject(current, size) {
        var _this3 = this;

        if (current.end) {
          this.removeFromStack(current);
          return undefined;
        }

        return readAsPromised(current.value, size).then(function (value) {
          if (value !== null) {
            if (!current.first) {
              _this3._push(',');
            }

            current.first = false;

            _this3.addToStack(value, undefined, current.readCount);

            current.readCount += 1;
          }
        });
      }
    }, {
      key: "processObject",
      value: function processObject(current) {
        // when no keys left, remove obj from stack
        if (!current.unread.length) {
          this.removeFromStack(current);
          return;
        }

        var key = current.unread.shift();
        var value = current.value[key];
        this.addToStack(value, key, undefined, current);
      }
    }, {
      key: "processArray",
      value: function processArray(current) {
        var key = current.unread;

        if (!key) {
          this.removeFromStack(current);
          return;
        }

        var index = current.arrayLength - key;
        var value = current.value[index];
        current.unread -= 1;
        this.addToStack(value, undefined, index, current);
      }
    }, {
      key: "processPrimitive",
      value: function processPrimitive(current) {
        if (current.value !== undefined) {
          var type = _typeof(current.value);

          var value;

          switch (type) {
            case 'string':
              value = quoteString(current.value);
              break;

            case 'number':
              value = Number.isFinite(current.value) ? String(current.value) : 'null';
              break;

            case 'bigint':
              value = String(current.value);
              break;

            case 'boolean':
              value = String(current.value);
              break;

            case 'object':
              if (!current.value) {
                value = 'null';
                break;
              }

            default:
              // This should never happen, I can't imagine a situation where this executes.
              // If you find a way, please open a ticket or PR
              throw Object.assign(new Error("Unknown type \"".concat(type, "\". Please file an issue!")), {
                value: current.value
              });
          }

          this._push(value);
        } else if (this.stack[1] && (this.stack[1].type === Types.Array || this.stack[1].type === Types.ReadableObject)) {
          this._push('null');
        } else {
          current.addSeparatorAfterEnd = false;
        }

        this.removeFromStack(current);
      }
    }, {
      key: "processReadableString",
      value: function processReadableString(current, size) {
        var _this4 = this;

        if (current.end) {
          this.removeFromStack(current);
          return undefined;
        }

        return readAsPromised(current.value, size).then(function (value) {
          if (value) _this4._push(escapeString(value.toString()));
        });
      }
    }, {
      key: "processPromise",
      value: function processPromise(current) {
        var _this5 = this;

        return recursiveResolve(current.value).then(function (value) {
          _this5.removeFromStack(current);

          _this5.addToStack(value, current.key, current.index, current.parent);
        });
      }
    }, {
      key: "processStackTopItem",
      value: function processStackTopItem(size) {
        var _this6 = this;

        var current = this.stack[0];
        if (!current || this.error) return Promise.resolve();
        var out;

        try {
          out = this[processFunctionLookupTable[current.type]](current, size);
        } catch (err) {
          return Promise.reject(err);
        }

        return Promise.resolve(out).then(function () {
          if (_this6.stack.length === 0) {
            _this6.end = true;

            _this6._push(null);
          }
        });
      } // tslint:disable-next-line:function-name

    }, {
      key: "__read",
      value: function __read(size) {
        var _this7 = this;

        if (this.isReading || this.error) {
          this.readMore = true;
          return undefined;
        }

        this.isReading = true; // we must continue to read while push has not been called

        this.readMore = false;
        return this.processStackTopItem(size).then(function () {
          var readAgain = !_this7.end && !_this7.error && (_this7.readMore || !_this7.pushCalled);

          if (readAgain) {
            setImmediate(function () {
              _this7.isReading = false;

              _this7.__read();
            });
          } else {
            _this7.isReading = false;
          }
        })["catch"](function (err) {
          _this7.error = true;

          _this7.emit('error', err);
        });
      } // tslint:disable-next-line:function-name

    }, {
      key: "_read",
      value: function _read(size) {
        this.pushCalled = false;

        this.__read(size);
      }
    }, {
      key: "path",
      value: function path() {
        return this.stack.map(function (_ref) {
          var key = _ref.key,
              index = _ref.index;
          return key || index;
        }).filter(function (v) {
          return v || v > -1;
        }).reverse();
      }
    }]);

    return JsonStreamStringify;
  }(stream.Readable);

  return JsonStreamStringify;

})));
//# sourceMappingURL=umd.js.map
