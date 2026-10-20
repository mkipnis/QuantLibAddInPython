#pragma once

#pragma once

#include <algorithm>
#include <type_traits>

#include <boost/shared_ptr.hpp>
#include <boost/json.hpp>

#include <ql/cashflow.hpp>
#include <ql/cashflows/coupon.hpp>
#include <ql/cashflows/fixedratecoupon.hpp>
#include <ql/cashflows/overnightindexedcoupon.hpp>
#include <ql/cashflows/simplecashflow.hpp>

namespace QuantLibAddinPython
{

namespace json = boost::json;

template<typename T>
json::value handle_cashflow(const T& cashflow)
{
    json::object cashflow_out;

    if constexpr (
        std::is_same_v<T, boost::shared_ptr<QuantLib::CashFlow>>
    )
    {
        if (!cashflow)
            return nullptr;

        const auto& cf = *cashflow;

        if (const auto* coupon =
                dynamic_cast<const QuantLib::FixedRateCoupon*>(&cf))
        {
            cashflow_out["type"] = "fixed_rate_coupon";
            cashflow_out["accrual_start_date"] =
                coupon->accrualStartDate().serialNumber();
            cashflow_out["accrual_end_date"] =
                coupon->accrualEndDate().serialNumber();
            cashflow_out["amount"] = coupon->amount();
            cashflow_out["rate"] = coupon->rate();
        }
        else if (const auto* coupon =
                     dynamic_cast<
                         const QuantLib::OvernightIndexedCoupon*
                     >(&cf))
        {
            cashflow_out["type"] = "overnight_indexed_coupon";
            cashflow_out["accrual_start_date"] =
                coupon->accrualStartDate().serialNumber();
            cashflow_out["accrual_end_date"] =
                coupon->accrualEndDate().serialNumber();
            cashflow_out["fixing_date"] =
                coupon->fixingDate().serialNumber();
            cashflow_out["accrual_period"] = coupon->accrualPeriod();
            cashflow_out["spread"] = coupon->spread();
            cashflow_out["amount"] = coupon->amount();
        }
        else if (const auto* redemption =
                     dynamic_cast<const QuantLib::Redemption*>(&cf))
        {
            cashflow_out["type"] = "redemption";
            cashflow_out["date"] = redemption->date().serialNumber();
            cashflow_out["amount"] = redemption->amount();
        }
        else if (const auto* simple_cashflow =
                     dynamic_cast<const QuantLib::SimpleCashFlow*>(&cf))
        {
            cashflow_out["type"] = "simple_cashflow";
            cashflow_out["date"] =
                simple_cashflow->date().serialNumber();
            cashflow_out["amount"] = simple_cashflow->amount();
        }
        else
        {
            cashflow_out["type"] = "other";
            cashflow_out["date"] = cf.date().serialNumber();
            cashflow_out["amount"] = cf.amount();
        }
    }

    return cashflow_out;
}

template<typename T>
json::value handle_leg(const T& leg)
{
    json::array cashflows;

    std::for_each(
        leg.begin(),
        leg.end(),
        [&cashflows](const auto& cashflow)
        {
            if (cashflow)
                cashflows.push_back(handle_cashflow(cashflow));
        }
    );

    return cashflows;
}

}
