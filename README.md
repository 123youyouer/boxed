1.EOS使用eosio::wallet_api_plugin插件管理钱包。可以在nodeos中加载这个插件实现钱包管理。
使用cleos测试的时候命令格式如下
./cleos --url http://127.0.0.1:8888 --wallet-url http://127.0.0.1:8888 set contract eosio ../../contracts/eosio.bios
2.void producer_plugin_impl::produce_block()是交易打包生成BLOCK的函数
chain.finalize_block();打包默克尔树（也许还打包了交易的具体数据？）
chain.sign_block签名
chain.commit_block();提交数据库，（出块？）
3.push_transaction
这里执行交易。系统内置的交易implicit=true；
外部的交易（智能合约产生的交易）会执行init_for_input_trx，exec，finalize后push_receipt
这里的push_receipt是放入需要打包里涉及的交易（第2条）。
4.新版会有检查数据库编译选项和代码编译选项是否一样的代码，注释掉就可以。
5.执行cleos，会判断来源的地址和配置的地址是否一样（字符串判断）比如127.0.0.1:8888和localhost:8888不一样就会返回失败

