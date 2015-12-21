# -*- coding: utf-8 -*-

# This file contains hook functions to run as the .feature file is executed.
#
# A common use-case is to use the OnScenarioStart/OnScenarioEnd hooks to
# start and stop an AUT, e.g.
#
# @OnScenarioStart
# def hook(context):
#     startApplication("addressbook")
#
# @OnScenarioEnd
# def hook(context):
#     currentApplicationContext().detach()
#
# See the section 'Performing Actions During Test Execution Via Hooks' in the Squish
# manual for a complete reference of the available API.

import __builtin__

# Detach (i.e. potentially terminate) all AUTs at the end of a scenario
@OnScenarioEnd
def hook(context):
    source(findFile("scripts", "common.py"))
    close_application()

    for ctx in applicationContextList():
        ctx.detach()

