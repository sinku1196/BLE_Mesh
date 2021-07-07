/*
 * rtable.h
 *
 */

#pragma once
#include <array>
#include <map>
#include <vector>
#include <ostream>

#include "bemesh_status.hpp"


namespace bemesh {
  #define DEV_ADDR_LEN 6
  typedef std::array<uint8_t, DEV_ADDR_LEN> dev_addr_t;
  dev_addr_t to_dev_addr(uint8_t *data);

  enum RoutingFlags {
    Reachable=0x01,
    Internet=0x02
  };

  struct routing_params_t {
    dev_addr_t target_addr;
    dev_addr_t hop_addr;
    uint8_t num_hops;
    uint8_t flags;

    inline bool operator==(const routing_params_t& rhs) {
      return (target_addr==rhs.target_addr)&&(hop_addr==rhs.hop_addr);}

    friend std::ostream& operator <<(std::ostream& os, const routing_params_t& up);
  };

  int print_dev_addr(char* buf, const dev_addr_t& addr);
  void print_routing_params(const routing_params_t& p);

  class RoutingTable {
    RoutingTable();
    std::map<dev_addr_t,routing_params_t > m_routing_table;

  public:
    static RoutingTable&getInstance();

    std::vector<routing_params_t> exportTable();

    routing_params_t insert(routing_params_t t_target_params);
    routing_params_t insert(dev_addr_t t_target_addr, dev_addr_t t_hop_addr, uint8_t t_num_hops,
			    uint8_t t_flags);

    ErrStatus remove(dev_addr_t t_target_addr);

    ErrStatus contains(dev_addr_t& t_target_addr);
    routing_params_t&get_routing_params(dev_addr_t t_target_addr);

    uint16_t size();
    int get_number_of_clients(dev_addr_t t_target_addr);

    std::size_t encodeTable(std::vector<routing_params_t>& t_src_vect, uint8_t* t_dest, std::size_t dest_len);
  };


}

