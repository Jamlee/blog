<?php
function logger($fileName) {
    $fileHandle = fopen($fileName, 'a');
    while (true) {
       fwrite($fileHandle, yield . "\n");
    }
}
$logger = logger('/dev/stdout');
while (true) {
  $logger->send('Foo');
  $logger->send('Bar');
  sleep(1);
}
?>
