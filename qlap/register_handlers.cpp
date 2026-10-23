#include <register_handlers.hpp>

#include <instrument_handlers.hpp>

#include <handlers/generic_handler.hpp>
#include <handlers/ois_handler.hpp>
#include <handlers/bond_handler.hpp>
#include <oh/repository.hpp>

#include <qlo/qladdin.hpp>
#include <qlo/overnightindexedswap.hpp>
#include <qlo/bonds.hpp>


#include <mutex>

namespace QuantLibAddinPython
{

void registerDefaultHandlers()
{
    static std::once_flag flag;

    std::call_once(flag, []()
    {
        // Register generic handler first.
        registerInstrumentHandler<QuantLibAddin::Instrument>(
            handleGenericInstrument);

        registerInstrumentHandler<
            QuantLibAddin::OvernightIndexedSwap>(
                handleOIS);

        registerInstrumentHandler<QuantLibAddin::Bond>(
            handleBond);
    });
}

}
