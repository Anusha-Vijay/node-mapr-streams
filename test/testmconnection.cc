#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <rdkafka.h>
#include <assert.h>

using namespace std;

TEST_CASE("create rdkafka conf", "[create_rdkafka_conf]") {
    rd_kafka_conf_t *_conf;
    _conf = rd_kafka_conf_new();
    assert(_conf != NULL);
    cout << "Conf created " << endl;
}