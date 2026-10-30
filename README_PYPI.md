### QuantLibAddIn Python Wrapper Overview

The QuantLibAddIn Python Wrapper provides a Python interface to QuantLibAddIn, exposing the functionality of the QuantLib C++ analytics library through an object-oriented API that close mirrors the architecture used by Excel-based QuantLibXL.

Example : OIS MidCurves
```
import quantlib_addin as qla
from datetime import datetime, timedelta


def main():
    try:
        # Initialize the environment
        qla.initializeAddin()

        print("Begin example program.")
        print("QuantLibAddin version =", qla.qlAddinVersion(False))
        print("ObjectHandler version =", qla.ohVersion(False))

        evaluation_date = 46218
        evaluation_date_str = datetime(year=1899, month=12, day=30) + timedelta(days=evaluation_date)
        print(f"Evaluation Date = {evaluation_date_str}")

        qla.qlSettingsSetEvaluationDate( evaluation_date, False,)

        fixing_days = 2
        rate_factor = 100

        sofr_index = qla.qlOvernightIndex(ObjectId="ois_index",
                                          FamilyName="SOFR",
                                          FixingDays=fixing_days,
                                          Currency="USD",
                                          Calendar="TARGET",
                                          DayCounter="Act/Act",
                                          YieldCurve = "",
                                          Permanent=False,
                                          Trigger=False,
                                          Overwrite=False)

        ois_curve_rates = {
            '5D': 5.0,
            '1W': 5.0,
            '2W': 5.0,
            '1M': 5.0,
            '3M': 5.0,
            '6M': 5.0,
            '9M': 5.0,
            '1Y': 5.0,
            '18M': 5.0,
            '2Y': 5.0,
            '3Y': 5.0,
            '5Y': 5.0,
            '10Y': 5.0,
            '20Y': 5.0,
            '30Y': 5.0
        }

        rate_helpers = []

        for swap_tenor, rate in ois_curve_rates.items():

            ois_rate_helper = qla.qlOISRateHelper(
                ObjectId=swap_tenor,
                SettlDays=2,
                Tenor=swap_tenor,
                FixedRate=rate/rate_factor,
                ONIndex=sofr_index,
                DiscountingCurve="",
                TelescopicValueDates=False,
                PaymentLag=0,
                Permanent=False,
                Trigger=False,
                Overwrite=False,
            )

            rate_helpers.append(ois_rate_helper)

        piecewise_ois_curve = qla.qlPiecewiseYieldCurve(
            ObjectId="piecewise_yieldcurve",
            NDays=fixing_days,
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

        qla.qlYieldTermStructureEnableExtrapolation(ObjectId=piecewise_ois_curve,
                                                    ExtrapolationFlag=True,
                                                    Trigger=True)

        engine = qla.qlDiscountingSwapEngine(
            "piecewise_ois_curve",
            YieldCurve=piecewise_ois_curve,
            IncludeSettlDate = False,
            SettlementDate=None,
            NpvDate=None,
            Permanent=False,
            Trigger=False,
            Overwrite=False,
        )

        sofr_index_with_curve = qla.qlOvernightIndex(ObjectId="ois_index_with_curve",
                                                     FamilyName="SOFR",
                                                     FixingDays=fixing_days,
                                                     Currency="USD",
                                                     Calendar="TARGET",
                                                     DayCounter="Act/Act",
                                                     YieldCurve=piecewise_ois_curve,
                                                     Permanent=False, Trigger=False, Overwrite=False)

        forward_start = [
            '0D', '1M', '3M', '6M', '1Y', '3Y', '5Y',
            '7Y', '10Y', '15Y', '20Y', '25Y', '30Y'
        ]

        swap_tenor = ['1Y', '3Y', '5Y', '7Y', '10Y', '15Y', '20Y', '25Y', '30Y']

        # Store results: rates[start][tenor]
        rates = {}

        for start in forward_start:
            rates[start] = {}

            for tenor in swap_tenor:
                ticker = start + "x" + tenor

                ois_swap = qla.qlMakeOIS(
                    ObjectId=ticker,
                    SettlDays=2,
                    SwapTenor=tenor,
                    OvernightIndex=sofr_index_with_curve,
                    FixedRate=0.000001,
                    ForwardStart=start,
                    FixDayCounter="Act/Act",
                    Spread=0.0,
                    PaymentLag=0,
                    Permanent=False,
                    Trigger=False,
                    Overwrite=False
                )

                qla.qlInstrumentSetPricingEngine(ois_swap, engine, False)

                results = qla.qlAddinCalculate(ois_swap)
                rates[start][tenor] = results['results']['fairRate']

        # Print table
        column_width = 10

        header = f"{'Forward':>{column_width}}"
        for tenor in swap_tenor:
            header += f"{tenor:>{column_width}}"

        print(header)
        print("-" * len(header))

        for start in forward_start:
            row = f"{start:>{column_width}}"
            for tenor in swap_tenor:
                # Display as percentage
                rate = rates[start][tenor] * 100
                row += f"{rate:>{column_width}.4f}"
            print(row)

    except Exception as e:
        print("Error:", e)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
```