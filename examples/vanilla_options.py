# Copyright (c) 2026 Mike Kipnis — QuantLibAddIn/Python

from pprint import pprint
import quantlib_addin as qla
from datetime import datetime,timedelta

def main():
    try:
        qla.initializeAddin()

        print("Begin example program.")
        print("QuantLibAddin version =", qla.qlAddinVersion(False))
        print("ObjectHandler version =", qla.ohVersion(False))

        evaluation_date = 46218

        evaluation_date_str = datetime(year=1899, month=12, day=30) + timedelta(days=evaluation_date)
        print(f"Evaluation Date = {evaluation_date_str}")

        qla.qlSettingsSetEvaluationDate(evaluation_date, False)

        exercise = qla.qlEuropeanExercise(ObjectId="exercise",
                                          ExpiryDate=evaluation_date + 10,
                                          Permanent=False, Trigger=False, Overwrite=False)

        striked_pay_off = qla.qlStrikedTypePayoff(ObjectId="qlStrikedTypePayoff",
                                                  PayoffID="Vanilla",
                                                  OptionType="Call",
                                                  Strike=45.75,
                                                  ThirdParameter=0,
                                                  Permanent=False, Trigger=False, Overwrite=False)

        vol_id = qla.qlBlackConstantVol(ObjectId="qlBlackConstantVol",
                                        SettlementDate=evaluation_date+1,
                                        Calendar="TARGET",
                                        Volatility=0.02,
                                        DayCounter="Act/Act",
                                        Permanent=False, Trigger=False, Overwrite=False)

        ql_generalized_black_scholes_process = qla.qlGeneralizedBlackScholesProcess(ObjectId="qlGeneralizedBlackScholesProcess",
                                                                                    BlackVolID=vol_id, Underlying=50,
                                                                                    DayCounter="Act/Act",
                                                                                    SettlementDate=evaluation_date+1,
                                                                                    RiskFreeRate=0.05,
                                                                                    DividendYield=0.1,
                                                                                    Permanent=False, Trigger=False, Overwrite=False)

        pricing_engine = qla.qlPricingEngine(ObjectId="PricingEngine",
                                             EngineID="AE",
                                             ProcessID=ql_generalized_black_scholes_process,
                                             Permanent=False, Trigger=False, Overwrite=False)

        option = qla.qlVanillaOption(ObjectId="VanillaOptions",
                                     Payoff=striked_pay_off,
                                     Exercise=exercise,
                                     Permanent=False, Trigger=False, Overwrite=False)

        qla.qlInstrumentSetPricingEngine(ObjectId=option,
                                         PricingEngine=pricing_engine,
                                         Trigger=False)

        option_npv = qla.qlInstrumentNPV(option, False)
        print(f"Option NPV: {option_npv}")

        option_results = qla.qlAddinCalculate(option)
        pprint(option_results)

        option_implied_vol = qla.qlAddinCalculate(option, {'price':5.25, 'process':ql_generalized_black_scholes_process})
        print(f"Implied Vol: (5.25) = {option_implied_vol['results']['impliedVolatility']}")


        option_implied_vol = qla.qlAddinCalculate(option,
                                                {'price':5.25,
                                                 'process':ql_generalized_black_scholes_process,
                                                 'dividends':{evaluation_date+5:0.6, evaluation_date+15:0.7}})
        print(f"Implied Vol: (5.25) + Div (0.06) = {option_implied_vol['results']['impliedVolatility']}")


    except Exception as e:
        print("Error:", e)
        return 1

    return 0

if __name__ == "__main__":
    raise SystemExit(main())