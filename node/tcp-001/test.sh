#!/bin/bash
token='K2pRkyqRVaMNTy8O4bblaXpaoTeGnVTm'
ts=$(date +%s)
nonce=$(openssl rand -hex 16)
siga=$ts$token$nonce$ts
sigb=$(echo -n $siga | sha256sum)
sigc=${sigb%%" "*}
echo $1
echo "创建供应商负责任账户:"
curl -v --location --request POST \
--header "x-tif-timestamp: $ts" \
--header 'x-tif-paasId: acc' \
--header "x-tif-nonce: $nonce" \
--header "x-tif-signature: $sigc" \
--header 'Content-Type: application/json' \
--data '{"app_id": "rio", "account_name": "initVenManagerOne"}' \
'http://localhost:8080/gogo/?version=2018-12-18' \
'http://localhost:8080/gogo/?version=2018-12-18'
echo ""