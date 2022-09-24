## todo list
* cout错误标准输出修改为日志类
* 接受线程改为多线程模式
* 引入协程
* 压力测试

## 问题
* 嵌套类和继承的区别
* monitor的作用

* ```c++
  template<typename T, typename D>
  bool ThreadQueue<T, D>::pop_front(T& t, size_t millsecond) {
      std::unique_lock<std::mutex> lock(mutex_);
      while(queue_.empty()) { //为什么if不行？
          cond_.wait(lock);
      }
      assert(!queue_.empty());
      t = queue_.front();
      queue_.pop_front();
      size_--;
      return true;
  }
  ```

* 
## 一些资料
https://cloud.tencent.com/developer/article/1663146?from=article.detail.1381523
