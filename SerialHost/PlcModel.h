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

      PlcModel()
      {
        for (auto& it : observed)
          it.resize(8);
      }

      void set(DataType dataType, unsigned index, bool value)
      {
        ObservableList& list = observed[static_cast<unsigned>(dataType)];

        if (index >= list.size())
          throw PlcException("index %d for type %d out of size", index, dataType);

        list[index].set(value);
        for (auto it = listObserver.begin(); it != listObserver.end(); it++)
          (*it)(dataType, index, value);
      }

      Observable<bool>& get(DataType dataType, unsigned index)
      {
        return observed[static_cast<unsigned>(dataType)][index];
      }

      const ObservableList& getList(DataType dataType)
      {
        return observed[static_cast<unsigned>(dataType)];
      }

      void addListObserver(ListObserver&& o)
      {
        listObserver.emplace_back(std::move(o));
      }

    private:

      std::array<ObservableList, 4> observed;
      std::vector<ListObserver> listObserver;
    };
  }
}
#endif // !_INCLUDE_PLC_MODEL_H_

