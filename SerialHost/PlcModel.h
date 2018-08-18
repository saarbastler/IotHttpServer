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

      Observable(T initial) : value(inital) {}

      typedef std::function<void(const T&)> Observer;

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

      void addObserver(Observer& o)
      {
        observerList.emplace_back();
        observerList.back() = o;
      }

    private:

      T value;

      std::vector<Observer> observerList;

    };

    enum class DataType
    {
      Inputs, Outputs, Merker, Monoflops
    };

    class PlcModel
    {
    public:

      using ObservableList = std::vector<Observable<bool>>;

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
      }

      Observable<bool> get(DataType dataType, unsigned index)
      {
        return observed[static_cast<unsigned>(dataType)][index];
      }

      const ObservableList& getList(DataType dataType)
      {
        return observed[static_cast<unsigned>(dataType)];
      }

    private:

      std::array<ObservableList, 4> observed;
    };
  }
}
#endif // !_INCLUDE_PLC_MODEL_H_

