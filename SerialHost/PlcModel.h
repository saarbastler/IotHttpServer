#ifndef _INCLUDE_PLC_MODEL_H_
#define _INCLUDE_PLC_MODEL_H_

#include <functional>
#include <vector>
#include <array>

namespace saba
{
  namespace plc
  {

    template<typename T>
    class Observable
    {
    public:

      Observable() {}

      Observable(T initial) : value(initial) {}

      using Observer= std::function<void(const T&)>;

      T operator () () const
      {
        return get();
      }

      T get() const
      {
        return value;
      }

      void set(T t)
      {
        value = t;
        for (auto it : observerList)
          it(value);
      }

      void addObserver(Observer&& o)
      {
        observerList.emplace_back();
        observerList.back() = std::move(o);
      }

    private:

      T value;

      std::vector<Observer> observerList;
    };

    template<typename T,typename U>
    class Observable2
    {
    public:
      Observable2() {}

      Observable2(T t, U u) : t(t), u(u) {}

      using Observer = std::function<void(const T&,const U&)>;

      T operator() () const
      {
        return getFirst();
      }

      T getFirst() const
      {
        return t;
      }

      U getSecond() const
      {
        return u;
      }

      void set(T t_, U u_)
      {
        t = t_;
        u = u_;
        for (auto it : observerList)
          it(t, u);
      }

      void addObserver(Observer&& o)
      {
        observerList.emplace_back(std::move(o));
      }

    private:

      T t;
      U u;

      std::vector<Observer> observerList;
    };

    template<typename T, typename U, typename V>
    class Observable3
    {
    public:
      Observable3() {}

      Observable3(T t, U u, V v) : t(t), u(u), v(v) {}

      using Observer = std::function<void(const T&, const U&, const V&)>;

      T operator() () const
      {
        return getFirst();
      }

      T getFirst() const
      {
        return t;
      }

      U getSecond() const
      {
        return u;
      }

      V getThird() const
      {
        return v;
      }

      void set(T t_, U u_, V v_)
      {
        t = t_;
        u = u_;
        v = v_;
        for (auto it : observerList)
          it(t, u, v);
      }

      void addObserver(Observer&& o)
      {
        observerList.emplace_back(std::move(o));
      }

    private:

      T t;
      U u;
      V v;

      std::vector<Observer> observerList;
    };

    enum class DataType
    {
      Inputs, Outputs, Merker, Monoflops
    };

    constexpr const char *DataTypeNames[] = { "input","output","merker","monoflop" };

    class PlcModel
    {
    public:

      using ObservableList = std::vector<Observable<bool>>;      
      using ListObserver = std::function<void(DataType,unsigned,bool)>;
      using MonoflopList = std::vector<Observable3<bool, unsigned, unsigned>>;
      using MonoflopObserver = std::function<void(unsigned,bool,unsigned,unsigned)>;

      PlcModel()
      {
        for (auto& it : observed)
          it.resize(8);

        observedMonoflops.resize(8);
      }

      void set(DataType dataType, unsigned index, bool value)
      {
        if (dataType == DataType::Monoflops)
          throw PlcException("use setMonoflop for Monoflop DataType");

        ObservableList& list = observed[static_cast<unsigned>(dataType)];

        if (index >= list.size())
          throw PlcException("index %d for type %d out of size", index, dataType);

        list[index].set(value);
        for (auto it = listObserver.begin(); it != listObserver.end(); it++)
          (*it)(dataType, index, value);
      }

      void setMonoflop(unsigned index, bool value, unsigned duration, unsigned remaining)
      {
        if (index >= observedMonoflops.size())
          throw PlcException("index %d for type Monoflop out of size", index);

        observedMonoflops[index].set(value, duration, remaining);
        for (auto& it : monoflopObserver)
          it(index, value, duration, remaining);
      }

      Observable<bool>& get(DataType dataType, unsigned index)
      {
        return observed[static_cast<unsigned>(dataType)][index];
      }

      const ObservableList& getList(DataType dataType)
      {
        return observed[static_cast<unsigned>(dataType)];
      }

      const MonoflopList& getMonoflopList()
      {
        return observedMonoflops;
      }

      void addListObserver(ListObserver&& o)
      {
        listObserver.emplace_back(std::move(o));
      }

      void addMonoflopObserver(MonoflopObserver&& o)
      {
        monoflopObserver.emplace_back(std::move(o));
      }

    private:

      std::array<ObservableList, 3> observed;
      std::vector<ListObserver> listObserver;

      MonoflopList observedMonoflops;
      std::vector<MonoflopObserver> monoflopObserver;
    };
  }
}
#endif // !_INCLUDE_PLC_MODEL_H_

