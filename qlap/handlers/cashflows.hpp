// Copyright (c) 2026 Mike Kipnis — QuantLibAddIn [Python]

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

    if constexpr ( std::is_same_v<T, boost::shared_ptr<QuantLib::CashFlow>> )
    {
        if (!cashflow)
            return nullptr;

        const auto& cf = *cashflow;

        if (const auto* coupon = dynamic_cast<const QuantLib::FixedRateCoupon*>(&cf))
        {
            cashflow_out["type"] = "FixedRateCoupon";
            cashflow_out["accrualStartDate"] = coupon->accrualStartDate().serialNumber();
            cashflow_out["accrualEndDate"] = coupon->accrualEndDate().serialNumber();
            cashflow_out["amount"] = coupon->amount();
            cashflow_out["rate"] = coupon->rate();
        }
        else if (const auto* coupon = dynamic_cast<const QuantLib::OvernightIndexedCoupon*>(&cf))
        {
            cashflow_out["type"] = "OvernightIndexedCoupon";
            cashflow_out["accrualStartDate"] = coupon->accrualStartDate().serialNumber();
            cashflow_out["accrualEndDate"] = coupon->accrualEndDate().serialNumber();
            cashflow_out["fixingDate"] = coupon->fixingDate().serialNumber();
            cashflow_out["accrualPeriod"] = coupon->accrualPeriod();
            cashflow_out["spread"] = coupon->spread();
            cashflow_out["amount"] = coupon->amount();
        }
        else if (const auto* redemption = dynamic_cast<const QuantLib::Redemption*>(&cf))
        {
            cashflow_out["type"] = "Redemption";
            cashflow_out["date"] = redemption->date().serialNumber();
            cashflow_out["amount"] = redemption->amount();
        }
        else if (const auto* simple_cashflow = dynamic_cast<const QuantLib::SimpleCashFlow*>(&cf))
        {
            cashflow_out["type"] = "SimpleCashFlow";
            cashflow_out["date"] = simple_cashflow->date().serialNumber();
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

    std::for_each( leg.begin(), leg.end(),
        [&cashflows](const auto& cashflow)
        {
            if (cashflow)
                cashflows.push_back(handle_cashflow(cashflow));
        }
    );

    return cashflows;
}

}
