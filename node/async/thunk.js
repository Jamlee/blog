fs = require('fs');

let fileName = './tmp.json'

// 正常版本的readFile（多参数版本）
// fs.readFile(fileName, callback);

var Thunk = function (func, fileName){
  let arg1 = function() {
    return fileName;
  }
  return function (callback){
    return func(arg1(), callback); 
  };
};
Thunk(fs.readFile, fileName)((err, data) => {
  console.log(data);
});

// 一个更通用的版本
var Thunk = function(fn){
  return function (){
    var args = Array.prototype.slice.call(arguments);
    return function (callback){
      args.push(callback);
      return fn.apply(this, args);
    }
  };
};
Thunk(fs.readFile, fileName)((err, data) => {
  console.log(data);
});
