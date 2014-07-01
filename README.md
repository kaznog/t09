#Photon for cocos2d-x AirHockey sample README

##本プログラムについて
本プログラムはcocos2d-x version 2.1.5にてPhoton Cloudを利用したリアルタイム同期ゲームのサンプルです。
解凍したフォルダをcocos2d-x 2.1.5のprojectsフォルダにコピーしてご利用いただくことを前提としたプロジェクト構造になっています。

##本プログラムの権利について
cocos2d-x AirHockeyはCocos2d-x by Example Beginner's Guideのサンプルゲームに
PhotonのCPP LoadBalancingランタイムを組み込んだ実装検証プログラムになります。
ゲーム実装部分の権利は著者であるRoger Engelbertに帰属します。

##PhotonCloudのAppID
Clases/Photon/NetworkLogic.cppのコンストラクタでAppIDを変更してご利用ください

##Androidでのビルド方法について
Androidでのビルドには、cocos2d-xのライブラリをADTプロジェクトにインポートし、本プロジェクトでライブラリ登録されている必要があります。
ADT_require_project.png　及びに　ADT_require_setting.pngのスクリーンショットを参考にADTにて設定してご利用ください。

(c)2014 Exit Games(R), all rights reserved. Powered by GMO CLOUD K.K.