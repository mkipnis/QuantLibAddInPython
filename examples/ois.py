# Copyright (c) 2026 Mike Kipnis — QuantLibAddIn/Python

from pprint import pprint

import quantlib_addin as qla


def main():
    try:
        # Initialize the environment
        qla.initializeAddin()

        print("Begin example program.")
        print("QuantLibAddin version =", qla.qlAddinVersion(False))
        print("ObjectHandler version =", qla.ohVersion(False))

        evaluation_date = 40546

        qla.qlSettingsSetEvaluationDate( evaluation_date, False,)

        sofr_index = qla.qlOvernightIndex("ois_index", "SOFR", 2, "USD", "TARGET", "Act/Act", "", False, False, False)

        rate_helpers = []

        fixing_dates = [
            qla.Property(evaluation_date-3),
            qla.Property(evaluation_date),
        ]

        fixing_values = [0.009, 0.01]

        qla.qlIndexAddFixings(
            "ois_index",
            fixing_dates,
            fixing_values,
            True,
            False,
        )

        ois_1Y = qla.qlOISRateHelper(
            ObjectId="OIS_1Y",
            SettlDays=2,
            Tenor="1Y",
            FixedRate=0.05,
            ONIndex=sofr_index,
            DiscountingCurve="",
            TelescopicValueDates=False,
            PaymentLag=0,
            Permanent=False,
            Trigger=False,
            Overwrite=False,
        )

        ois_3Y = qla.qlOISRateHelper(
            ObjectId="OIS_3Y",
            SettlDays=2,
            Tenor="3Y",
            FixedRate=0.05,
            ONIndex=sofr_index,
            DiscountingCurve="",
            TelescopicValueDates=False,
            PaymentLag=0,
            Permanent=False,
            Trigger=False,
            Overwrite=False,
        )

        ois_5Y = qla.qlOISRateHelper(
            ObjectId="OIS_5Y",
            SettlDays=2,
            Tenor="5Y",
            FixedRate=0.05,
            ONIndex=sofr_index,
            DiscountingCurve="",
            TelescopicValueDates=False,
            PaymentLag=0,
            Permanent=False,
            Trigger=False,
            Overwrite=False,
        )

        ois_10Y = qla.qlOISRateHelper(
            ObjectId="OIS_10Y",
            SettlDays=2,
            Tenor="10Y",
            FixedRate=0.05,
            ONIndex=sofr_index,
            DiscountingCurve="",
            TelescopicValueDates=False,
            PaymentLag=0,
            Permanent=False,
            Trigger=False,
            Overwrite=False,
        )

        ois_30Y = qla.qlOISRateHelper(
            ObjectId="OIS_30Y",
            SettlDays=2,
            Tenor="30Y",
            FixedRate=0.05,
            ONIndex=sofr_index,
            DiscountingCurve="",
            TelescopicValueDates=False,
            PaymentLag=0,
            Permanent=False,
            Trigger=False,
            Overwrite=False,
        )

        rate_helpers.append(ois_1Y)
        rate_helpers.append(ois_3Y)
        rate_helpers.append(ois_5Y)
        rate_helpers.append(ois_10Y)
        rate_helpers.append(ois_30Y)

        curve = qla.qlPiecewiseYieldCurve(
            ObjectId="piecewise_yieldcurve",
            NDays=2,
            Calendar="TARGET",
            RateHelpers=rate_helpers,
            DayCounter="Actual/360",
            Jumps=[],
            JumpDates=[],
            TraitsID="Discount",
            InterpolatorID="LogLinear",
            Permanent=False,
            Trigger=False,
            Overwrite=False,
        )

        qla.qlYieldTermStructureEnableExtrapolation(curve, True, True)

        engine = qla.qlDiscountingSwapEngine(
            "DiscountingSwapEngine",
            curve,  # YieldCurve
            False,  # IncludeSettlementDateFlows
            None,  # SettlementDate
            None,  # NpvDate
            False,  # Permanent
            False,  # Trigger
            False,  # Overwrite
        )

        sofr_index_with_curve = qla.qlOvernightIndex("ois_index_with_curve",
                                                     "SOFR", 2, "USD", "TARGET", "Act/Act", curve, False, False, False)

        ois_swap = qla.qlMakeOIS(
            ObjectId = "my_ois",
            SettlDays = 2,
            SwapTenor = "50Y",
            OvernightIndex = sofr_index_with_curve,
            FixedRate = 0.03,
            ForwardStart = "0D",
            FixDayCounter = "Act/Act",
            Spread = 0.0,
            PaymentLag = 0,
            Permanent = False,
            Trigger = False,
            Overwrite = False
        )

        qla.qlInstrumentSetPricingEngine(
            ois_swap,
            engine,
            False,
        )

        npv = qla.qlInstrumentNPV(ois_swap, False)
        print(f"OIS NPV : {npv}")

        results = qla.qlAddinCalculate(ois_swap)

        pprint(results, indent=4, sort_dicts=False)

    except Exception as e:
        print("Error:", e)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())