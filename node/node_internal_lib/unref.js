// 不等待timeout就退出
d = setTimeout(() => { console.log('hello')}, 30000);
d.unref();