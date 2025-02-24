/*
 * rrLLVMEvalRateRuleRatesCodeGen.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: andy
 */
#pragma hdrstop
#include "EvalRateRuleRatesCodeGen.h"
#include "LLVMException.h"
#include "ASTNodeCodeGen.h"
#include "ASTNodeFactory.h"
#include "ModelDataSymbolResolver.h"
#include "rrLogger.h"
#include <sbml/math/ASTNode.h>
#include <sbml/math/FormulaFormatter.h>
#include <Poco/Logger.h>


using namespace libsbml;
using namespace llvm;



namespace rrllvm
{

const char* EvalRateRuleRatesCodeGen::FunctionName = "evalRateRuleRates";

EvalRateRuleRatesCodeGen::EvalRateRuleRatesCodeGen(
        const ModelGeneratorContext &mgc) :
        CodeGenBase<EvalRateRuleRates_FunctionPtr>(mgc)
{
}

EvalRateRuleRatesCodeGen::~EvalRateRuleRatesCodeGen()
{
}

Value* EvalRateRuleRatesCodeGen::codeGen()
{
    Value *modelData = 0;

    codeGenVoidModelDataHeader(FunctionName, modelData);

    ModelDataLoadSymbolResolver resolver(modelData, modelGenContext);
    ModelDataIRBuilder mdbuilder(modelData, dataSymbols, builder);
    ASTNodeCodeGen astCodeGen(builder, resolver, modelGenContext, modelData);
    ASTNodeFactory nodes;

    // iterate through all of the reaction, and generate code based on thier
    // kinetic rules.

    model->getListOfRules();
    const ListOfRules *rules = model->getListOfRules();

    for (int i = 0; i < rules->size(); ++i)
    {
        const ASTNode* math = 0;
        const Rule* rule = rules->get(i);
        if (rule->getTypeCode() == SBML_RATE_RULE)
        {
            const RateRule* rateRule = static_cast<const RateRule*>(rule);
            // check if this rate rule applies to species, we only deal with
            // amounts and rates of change of amounts, so need to convert
            // accordingly
            const Species* species = const_cast<Model*>(model)->getSpecies(rateRule->getVariable());
            if (species)
            {
                if (!species->getHasOnlySubstanceUnits())
                {
                    // product rule, need to check if we have a rate rule for the
                    // species compartment.
                    const Rule* comprule = rules->get(species->getCompartment());
                    if (comprule && comprule->getTypeCode() == SBML_RATE_RULE)
                    {
                        const RateRule* compRateRule = static_cast<const RateRule*>(comprule);
                        rrLog(Logger::LOG_DEBUG) << "species " << species->getId()
                                << " is a concentration with time dependent volume, "
                                "converting conc rate to amt rate using product rule";
                        ASTNode *dcdt = new ASTNode(*compRateRule->getMath());
                        ASTNode *c = new ASTNode(AST_NAME);
                        c->setName(species->getCompartment().c_str());

                        ASTNode *dvdt = new ASTNode(*rateRule->getMath());
                        ASTNode *v = new ASTNode(AST_NAME);
                        v->setName(species->getId().c_str());

                        ASTNode *l = new ASTNode(AST_TIMES);
                        l->addChild(v);
                        l->addChild(dcdt);

                        ASTNode *r = new ASTNode(AST_TIMES);
                        r->addChild(c);
                        r->addChild(dvdt);

                        ASTNode *plus = nodes.create(AST_PLUS);
                        plus->addChild(l);
                        plus->addChild(r);

                        math = plus;
                    }
                    else
                    {
                        rrLog(Logger::LOG_DEBUG) << "species " << species->getId()
                                << " is a concentration with constant volume, "
                                "converting conc rate to amt rate const vol mul";

                        ASTNode *dcdt = new ASTNode(*rateRule->getMath());
                        ASTNode *v = new ASTNode(AST_NAME);
                        v->setName(species->getCompartment().c_str());

                        ASTNode *times = nodes.create(AST_TIMES);
                        times->addChild(dcdt);
                        times->addChild(v);

                        math = times;
                    }
                }
                else
                {
                    rrLog(Logger::LOG_DEBUG) << "species " << species->getId() <<
                            " is an amount, creating straight rate rule";
                    math = rateRule->getMath();
                }
            }
            else
            {
                math = rateRule->getMath();
            }
            assert(math);
            Value *value = astCodeGen.codeGenDouble(math);

            mdbuilder.createRateRuleRateStore(rateRule->getVariable(), value);
        }
    }

    builder.CreateRetVoid();

    return verifyFunction();
}

} /* namespace rr */
