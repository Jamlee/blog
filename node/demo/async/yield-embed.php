<?php
// https://www.laruence.com/2015/05/28/3038.html
// 如果嵌套yield会怎么样呢
function test_1_100() {
  for ($i=1; $i<=100; $i++) {
       yield $i;
  }
}
function test_100_200() {
  yield test_1_100();
  for ($i=100; $i<=200; $i++) {
       yield $i;
  }
}

// Recoverable fatal error: Object of class Generator could not be converted to string in /Users/jamlee/Store/dev/blog/node/async/yield-embed.php on line 17
// $generator = test_100_200();
// foreach ($generator as $value) {
//   echo "$value\n";
// }

// 函数参数中使用 yield
// function add($a, $b) {
//   $x = $a + $b;
//   return $x;
// }
// function testFunc() {
//    add(1, yield add(1, 1));
// }
// $t = testFunc();
// var_dump($t->current());

// 用了这个 return 将没啥用了
function stackedCoroutine(\Generator $gen)
{
    $stack = new \SplStack;
    $exception = null;
    for (;;) {
        try {
            if ($exception) {
                $gen->throw($exception);
                $exception = null;
                continue;
            }
            // 执行一次 yield
            $value = $gen->current();

            if ($value instanceof \Generator) {
                // 压入当前的 $gen, 先把嵌套的 $gen 执行
                $stack->push($gen);
                $gen = $value;
                continue;
            }
            // 当前的 gen 函数是否执行完毕了
            $isReturnValue = $value instanceof CoroutineReturnValue;
            if (!$gen->valid() || $isReturnValue) {
                if ($stack->isEmpty()) {// 堆栈已空
                    return;
                }
                // 找到之前的 gen
                $gen = $stack->pop();
                // 把值发送到上个gen函数的yield表达式上
                $gen->send($isReturnValue ? $value->getValue() : NULL);
                continue;
            }
            // 当前的gen还没有执行完毕
            try {
                // yield 一对 key value
                // https://www.php.net/manual/en/class.generator.php
                $sendValue = (yield $gen->key() => $value);
            } catch (\Exception $e) {
                $gen->throw($e);
                continue;
            }
            // 发送收敛后的值
            $gen->send($sendValue);
        } catch (\Exception $e) {
            if ($stack->isEmpty()) {
                throw $e;
            }
            $gen = $stack->pop();
            $exception = $e;
        }
    }
}
// 函数参数嵌套yield
function add($a, $b) {
  echo "a:$a b:$b\n";
  return $a + $b;
}
function yield_value() {
  return 10;
}
function testFunc() {
  $res = yield add(yield yield_value(), 20);
}
$t = stackedCoroutine(testFunc());
echo "第一次：" . $t->current() . "\n";
$t->send($t->current());
echo "第二次：" . $t->current() . "\n";
$t->send();
if (!$t->valid()) {
  echo "generator is closed\n";
}
?>


