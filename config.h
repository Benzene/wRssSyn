#ifndef _WRSSSYN_CONFIG_H_
#define _WRSSSYN_CONFIG_H_

#include "genericConfig.h"

class Config : public GenericConfig {

    private:
        static GenericConfig * instance;
        Config();
        static GenericConfig * init();

    public:
        static GenericConfig * get_instance();

};

#endif
