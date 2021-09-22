#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <string>
using std::cout;
using std::endl;
using std::list;
using std::string;
using std::unique_ptr;

class Observer;

struct Status {
  virtual ~Status() {}
};

struct RingStatus : public Status {
  bool _isAlarming;
};

class Subject {
 public:
  Subject() : _status(nullptr) {}

  virtual void attach(Observer* ob) = 0;
  virtual void detach(Observer* ob) = 0;
  virtual void notify() = 0;

  virtual Status& getStatus() { return *_status; }

  virtual void setStatus(Status status) {
    Status* ptmp = new Status(status);
    _status = ptmp;
  }
  virtual ~Subject() {
    if (_status) {
      delete _status;
    }
  }

  Status* _status;
};

class Ring : public Subject {
 public:
  Ring() {}

  void bePressed() {
    cout << ">> Ring is bepressed!" << endl;

    notify();
  }

  virtual void attach(Observer* ob) {
    if (ob) {
      auto it = std::find(_obList.begin(), _obList.end(), ob);
      //如果没有查找到ob,就添加到obList之中
      if (it == _obList.end()) {
        _obList.push_back(ob);
      }
    }
  }

  virtual void detach(Observer* ob) {
    if (ob) {
      auto it = std::find(_obList.begin(), _obList.end(), ob);
      //如果查找到ob,就从obList之中删除
      if (it != _obList.end()) {
        _obList.erase(it);
      }
    }
  }

  virtual void notify();

 private:
  //用户类型可以进行细分
  list<Observer*> _obList;
  list<Observer*> _goldenList;
  list<Observer*> _silverList;
  list<Observer*> _ptList;
};

class Guest {
 public:
  Guest(const string& name) : _name(name) {}

  void knockAtDoor(Ring& ring) {
    cout << ">> 客人 " << _name << "is Koncking at the door！" << endl;
    Status& s = ring.getStatus();
    RingStatus* rs = dynamic_cast<RingStatus*>(&s);
    if (rs) {
      rs->_isAlarming = true;
    }
    ring.setStatus(*rs);  // 改变好的状态设置给了Ring

    ring.bePressed();
  }

 private:
  string _name;
};

class Observer {
 public:
  Observer(const string& name) : _name(name) {}

  virtual void update() = 0;
  //给信息本身 ==> 观察者模式中的推模式(被动接收)
  virtual void update(const string& msg);
  //给主题的指针或引用 ==> 观察者模式中的拉模式(主动获取)
  virtual void update(Subject* subject);

 protected:
  string _name;
};

void Ring::notify() {
  for (auto& ob : _obList) {
    ob->update();
  }
}

class Baby : public Observer {
 public:
  Baby(const string& name) : Observer(name) {}

  virtual void update() {
    // Subject * subject
    // subject->getStatus();//获取主题的具体信息

    srand(clock());
    int randomNumber = rand() % 100;
    if (randomNumber > 70) {
      cout << "> Baby " << _name << " starts crying loudly!" << endl;
    } else {
      cout << "> Baby " << _name << " is sleeping sweetly!" << endl;
    }
  }
};

class Nurse : public Observer {
 public:
  Nurse(const string& name) : Observer(name) {}

  virtual void update() {
    srand(clock());
    int randomNumber = rand() % 100;
    cout << "randomNumber: " << randomNumber << endl;
    if (randomNumber > 70) {
      cout << "> Nurse " << _name << " is sleeping sweetly!" << endl;
    } else {
      cout << "> Nurse " << _name << " is ready to open the door!" << endl;
    }
  }
};

int main(void) {
  unique_ptr<Observer> baby1(new Baby("haa"));
  unique_ptr<Observer> baby2(new Baby("bb"));
  unique_ptr<Observer> nurse1(new Nurse("ccc"));
  unique_ptr<Observer> nurse2(new Nurse("asfsf"));

  Ring ring;
  RingStatus status;
  status._isAlarming = false;
  ring.setStatus(status);

  ring.attach(baby1.get());
  ring.attach(baby2.get());
  ring.attach(nurse1.get());
  ring.attach(nurse2.get());
  Guest guest("abb");
  guest.knockAtDoor(ring);
  cout << endl << endl;

  ring.detach(nurse1.get());
  guest.knockAtDoor(ring);

  return 0;
}
