queue
=====
### 队列特点：<br/>
     1. 持久化存储，顺序的读写；
     2. 支持优先级别读取，存在0,1,2,3,4,5,6,7,8,9 这十个级别，数字越小优先级越低，如果读的时候不指定级别
        会读取存在数据的最高级别。
### 调用实例
     //包含头文件
     #include "include/queue.h"
     //创建Queue对象
     levelque::Queue *db;
     //打开一个bbs队列，目录为/tmp
     levelque::Queue::Open("/tmp","bbs", &db);
     
     //创建一个新的队列
     db->CreateQueueName("/tmp", "blog");
     
### 写数据
     //向bbs队列写入数据
     char testData[100]={0};
     sprintf(testData, "%ld_welcome to beijing_1",time(NULL));
     //向级别bbs队列中的1级别写入数据
     db->Write("bbs",testdata, strlen(testData),1);
     //向级别bbs队列中的 <b>9级别</b>写入数据
     db->Write("bbs",testdata, strlen(testData),9);

### 读数据
     //默认读取级别高的，且有数据的 bbs队列
     if(db->Read("bbs", data)) {
          cout << "max read: " << data << endl;
          cout << "the number of bbs queue: "<< db->Size("bbs") << endl;
     }
     //读取指定级别的数据
     db->Read("abc_two", data, 2);
     
### 读取队列的数量
     //bbs队列中的数据条数
     db->Size("bbs");
     //指定级别中的数据条数
     db->Size("bbs", 2);
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
      
