from pprint import pprint

import quantlib_addin as qla


def main():
    try:
        # Initialize the environment
        qla.initializeAddin()

        print("Begin example program.")
        print("QuantLibAddin version =", qla.qlAddinVersion(False))
        print("ObjectHandler version =", qla.ohVersion(False))

        # ------------------------------------------------------------
        # Set the evaluation date to 1 January 2011
        # ------------------------------------------------------------
        evaluation_date = 40546

        qla.qlSettingsSetEvaluationDate( evaluation_date, False,)
        # ------------------------------------------------------------
        # Create the market data objects
        # ------------------------------------------------------------

        flat_forward = qla.qlFlatForward(
            "FlatForward",
            0,  # NDays
            "NullCalendar",  # Calendar
            0.044,  # Rate
            "Actual/365 (Fixed)",  # DayCounter
            "Continuous",  # Compounding
            "Annual",  # Frequency
            False,  # Permanent
            False,  # Trigger
            False,  # Overwrite
        )

        # ------------------------------------------------------------
        # Create schedules
        # ------------------------------------------------------------
        effective_date = 40548
        termination_date = 44201

        bond_schedule = qla.qlSchedule(
            "BondSchedule",
            effective_date,
            termination_date,
            "6M",
            "TARGET",
            "Modified Following",
            "Modified Following",
            "Backward",
            False,  # FirstDate
            None,  # NextToLastDate
            None,  # EndOfMonth
            False,  # Permanent
            False,  # Trigger
            False,  # Overwrite
        )

        engine = qla.qlBondEngine(
            "DiscountingSwapEngine",
            flat_forward,  # YieldCurve
            False,  # Permanent
            False,  # Trigger
            False,  # Overwrite
        )

        coupons = [0.05]

        fixed_rate_bond = qla.qlFixedRateBond(
            ObjectId = "FixedRateBond",
            Description = "FixedRateBond-TEST",
            Currency = "USD",
            SettlementDays = 1,
            FaceAmount = 100.0,
            ScheduleID = bond_schedule,
            Coupons = coupons,
            DayCounter = "Act/Act",
            PaymentBDC = "Modified Following",
            Redemption = 100.0,
            IssueDate = effective_date,
            PaymentCalendar = "TARGET",
            Permanent = False,
            Trigger = False,
            Overwrite = False
        )

        # ------------------------------------------------------------
        # Set the pricing engine
        # ------------------------------------------------------------
        qla.qlInstrumentSetPricingEngine(
            fixed_rate_bond,
            engine,
            False,
        )

        results = qla.qlAddinCalculate(fixed_rate_bond, {})

        pprint(results, indent=4, sort_dicts=False)

        #  Price to Yield
        price_to_yield_setup = {'price':105.015625}
        price_to_yield = qla.qlAddinCalculate(fixed_rate_bond, price_to_yield_setup)
        results = price_to_yield["results"]
        print(f"Price: {results['cleanPrice']} -> Yield: {results['yield']}")

        # Yield to Price
        yield_to_price_setup = {'yield':0.05}
        yield_to_price = qla.qlAddinCalculate(fixed_rate_bond, yield_to_price_setup)
        results = yield_to_price["results"]
        print(f"Yield: {results['yield']} -> Price: {results['cleanPrice']}")

        # Z-Spread
        # Flat 4% SOFR curve
        sofr_rate = 0.04

        flat_forward_sofr = qla.qlFlatForward(
            "FlatForwardSOFR",
            0,  # NDays
            "TARGET",  # Calendar
            sofr_rate,  # Rate
            "Actual/360",  # DayCounter
            "Continuous",  # Compounding
            "Annual",  # Frequency
            False,  # Permanent
            False,  # Trigger
            False,  # Overwrite
        )

        sofr_index_with_curve = qla.qlOvernightIndex("ois_index_with_curve",
                                                     "SOFR", 2, "USD", "TARGET", "Act/Act", flat_forward_sofr, False, False, False)

        zspread_setup = {'price':100.015625, 'zcurve': flat_forward_sofr}
        zspread = qla.qlAddinCalculate(fixed_rate_bond, zspread_setup)
        results = zspread["results"]
        print(f"Price: {results['cleanPrice']} -> Yield: {results['yield']} -> Z-Spread: {results['zspread']}")

        print("End example program.")

    except Exception as e:
        print("Error:", e)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())