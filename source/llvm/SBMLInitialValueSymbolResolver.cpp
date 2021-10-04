/*
 * SBMLInitialValueSymbolResolver.cpp
 *
 *  Created on: Jul 25, 2013
 *      Author: andy
 */
#pragma hdrstop
#include "SBMLInitialValueSymbolResolver.h"
#include "ASTNodeCodeGen.h"
#include "LLVMException.h"
#include "FunctionResolver.h"
#include <sbml/Model.h>


using namespace libsbml;
using namespace llvm;

namespace rrllvm
{

SBMLInitialValueSymbolResolver::SBMLInitialValueSymbolResolver(
        llvm::Value *modelData,
        const ModelGeneratorContext& ctx) :
        LoadSymbolResolverBase(ctx, modelData), modelData(modelData)
{
}

llvm::Value* SBMLInitialValueSymbolResolver::loadSymbolValue(
        const std::string& symbol,
        const llvm::ArrayRef<llvm::Value*>& args)
{
    /*************************************************************************/
    /* time */
    /*************************************************************************/
    if (symbol.compare(SBML_TIME_SYMBOL) == 0)
    {
        return ConstantFP::get(builder.getContext(), APFloat(0.0));
    }

    /*************************************************************************/
    /* Function */
    /*************************************************************************/
    {
        Value *funcVal =
            FunctionResolver(*this, modelData, modelGenContext).loadSymbolValue(symbol, args);

        if (funcVal)
        {
            return funcVal;
        }
    }

    /*************************************************************************/
    /* RateOf */
    /*************************************************************************/
    {
        ModelDataIRBuilder mdbuilder(modelData, modelDataSymbols, builder);
        if (symbol[symbol.size() - 1] == '\'')
        {
            Value* rate = 0;
            string subsymbol = symbol.substr(0, symbol.size() - 1);
            //Looking for a rate.  Our options are: floating species, rate rule target, assignment rule target (an error), and everything else is zero.
            if (modelDataSymbols.isIndependentFloatingSpecies(subsymbol))
            {
                rate = mdbuilder.createFloatSpeciesAmtRateLoad(subsymbol, subsymbol + "_rate");
            }
            else if (modelDataSymbols.hasRateRule(subsymbol))
            {
                rate = mdbuilder.createRateRuleRateLoad(subsymbol, subsymbol + "_rate");
            }
            else if (modelDataSymbols.hasAssignmentRule(subsymbol))
            {
                throw_llvm_exception("Unable to define the rateOf for symbol '" + symbol + "' as it is changed by an assignment rule.");
            }
            else
            {
                return ConstantFP::get(builder.getContext(), APFloat(0.0));
            }
            assert(rate);
            return cacheValue(symbol, args, rate);
        }
    }
    /*************************************************************************/
    /* AssignmentRule */
    /*************************************************************************/
    if (!modelDataSymbols.isConservedMoietySpecies(symbol))
    {
        SymbolForest::ConstIterator i =
                modelSymbols.getAssigmentRules().find(symbol);
        if (i != modelSymbols.getAssigmentRules().end())
        {
            recursiveSymbolPush(symbol);
            Value* result = ASTNodeCodeGen(builder, *this, modelGenContext, modelData).codeGen(i->second);
            recursiveSymbolPop();
            return result;
        }
    }

    /*************************************************************************/
    /* Initial Value */
    /*************************************************************************/
    {
        SymbolForest::ConstIterator i =
                modelSymbols.getInitialValues().find(symbol);

        if (i != modelSymbols.getInitialValues().end())
        {
            return ASTNodeCodeGen(builder, *this, modelGenContext, modelData).codeGen(i->second);
        }
    }

    /*************************************************************************/
    /* Reaction Rate */
    /*************************************************************************/
    const Reaction* reaction = model->getReaction(symbol);
    if (reaction)
    {
        return loadReactionRate(reaction);
    }

    std::string msg = "Could not find requested symbol \'";
    msg += symbol;
    msg += "\' in the model";
    throw_llvm_exception(msg);

    return 0;
}

} /* namespace rr */


