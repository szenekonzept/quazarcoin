#include <string>

class Minerc;

class MinerHandle {
public:
  MinerHandle();
  ~MinerHandle();

  bool StartMining(const std::string& address1, const std::string& address2, size_t threads, const std::string& wallet1, const std::string& wallet2);
  void StopMining();
  double GetHashRate();

private:
  Minerc *impl;
};
