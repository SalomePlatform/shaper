# Copyright (C) 2020-2024  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See https://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

### Test offset using approximation
### bos #20476 EDF 22546 - sigsegv with offset

from salome.shaper import model

#=========================================================================
# Creation of a document and sketch
#=========================================================================

model.begin()
partSet = model.moduleDocument()

### Create Sketch
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))


#=========================================================================
# Creation of a testing B-spline
#=========================================================================

### Create SketchPoint
SketchPoint_1 = Sketch_1.addPoint(0, 0)
SketchPoint_1.setAuxiliary(True)

### Create SketchPoint
SketchPoint_2 = Sketch_1.addPoint(0.03453184, 0.15311942)
SketchPoint_2.setAuxiliary(True)

### Create SketchPoint
SketchPoint_3 = Sketch_1.addPoint(0.11839488, 0.28145762)
SketchPoint_3.setAuxiliary(True)

### Create SketchPoint
SketchPoint_4 = Sketch_1.addPoint(0.20229646, 0.3653592)
SketchPoint_4.setAuxiliary(True)

### Create SketchPoint
SketchPoint_5 = Sketch_1.addPoint(0.325663, 0.44425058)
SketchPoint_5.setAuxiliary(True)

### Create SketchPoint
SketchPoint_6 = Sketch_1.addPoint(0.44409642, 0.5083426)
SketchPoint_6.setAuxiliary(True)

### Create SketchPoint
SketchPoint_7 = Sketch_1.addPoint(0.5625298400000001, 0.5526636)
SketchPoint_7.setAuxiliary(True)

### Create SketchPoint
SketchPoint_8 = Sketch_1.addPoint(0.70069574, 0.5969460600000001)
SketchPoint_8.setAuxiliary(True)

### Create SketchPoint
SketchPoint_9 = Sketch_1.addPoint(0.8684989, 0.63132374)
SketchPoint_9.setAuxiliary(True)

### Create SketchPoint
SketchPoint_10 = Sketch_1.addPoint(1.02146416, 0.6509020600000001)
SketchPoint_10.setAuxiliary(True)

### Create SketchPoint
SketchPoint_11 = Sketch_1.addPoint(1.22873228, 0.65063228)
SketchPoint_11.setAuxiliary(True)

### Create SketchPoint
SketchPoint_12 = Sketch_1.addPoint(1.41129626, 0.6454293800000001)
SketchPoint_12.setAuxiliary(True)

### Create SketchPoint
SketchPoint_13 = Sketch_1.addPoint(1.58403254, 0.6205325399999999)
SketchPoint_13.setAuxiliary(True)

### Create SketchPoint
SketchPoint_14 = Sketch_1.addPoint(1.7369978, 0.60550194)
SketchPoint_14.setAuxiliary(True)

### Create SketchPoint
SketchPoint_15 = Sketch_1.addPoint(1.9244949, 0.5632621)
SketchPoint_15.setAuxiliary(True)

### Create SketchPoint
SketchPoint_16 = Sketch_1.addPoint(2.10709742, 0.5160891400000001)
SketchPoint_16.setAuxiliary(True)

### Create SketchPoint
SketchPoint_17 = Sketch_1.addPoint(2.27486204, 0.4763544)
SketchPoint_17.setAuxiliary(True)

### Create SketchPoint
SketchPoint_18 = Sketch_1.addPoint(2.4179996, 0.43661966)
SketchPoint_18.setAuxiliary(True)

### Create SketchPoint
SketchPoint_19 = Sketch_1.addPoint(2.55123238, 0.39692346)
SketchPoint_19.setAuxiliary(True)

### Create SketchPoint
SketchPoint_20 = Sketch_1.addPoint(2.70913076, 0.3522556)
SketchPoint_20.setAuxiliary(True)

### Create SketchPoint
SketchPoint_21 = Sketch_1.addPoint(2.89173328, 0.29768296)
SketchPoint_21.setAuxiliary(True)

### Create SketchPoint
SketchPoint_22 = Sketch_1.addPoint(3.04469854, 0.2530151)
SketchPoint_22.setAuxiliary(True)

### Create SketchPoint
SketchPoint_23 = Sketch_1.addPoint(3.19273068, 0.20341412)
SketchPoint_23.setAuxiliary(True)

### Create SketchPoint
SketchPoint_24 = Sketch_1.addPoint(3.35556218, 0.1537746)
SketchPoint_24.setAuxiliary(True)

### Create SketchPoint
SketchPoint_25 = Sketch_1.addPoint(3.50363286, 0.10914528)
SketchPoint_25.setAuxiliary(True)

### Create SketchPoint
SketchPoint_26 = Sketch_1.addPoint(3.63686564, 0.06944908000000001)
SketchPoint_26.setAuxiliary(True)

### Create SketchPoint
SketchPoint_27 = Sketch_1.addPoint(3.7404997, 0.03965766)
SketchPoint_27.setAuxiliary(True)

### Create SketchPoint
SketchPoint_28 = Sketch_1.addPoint(3.838584, 0.004933120000000001)
SketchPoint_28.setAuxiliary(True)

### Create SketchPoint
SketchPoint_29 = Sketch_1.addPoint(3.854, -0.009866240000000002)
SketchPoint_29.setAuxiliary(True)

### Create SketchPoint
SketchPoint_30 = Sketch_1.addPoint(3.77009842, -0.004817500000000001)
SketchPoint_30.setAuxiliary(True)

### Create SketchPoint
SketchPoint_31 = Sketch_1.addPoint(3.56040228, 0.01025164)
SketchPoint_31.setAuxiliary(True)

### Create SketchPoint
SketchPoint_32 = Sketch_1.addPoint(3.43946376, 0.00300612)
SketchPoint_32.setAuxiliary(True)

### Create SketchPoint
SketchPoint_33 = Sketch_1.addPoint(3.31609722, -0.02154386)
SketchPoint_33.setAuxiliary(True)

### Create SketchPoint
SketchPoint_34 = Sketch_1.addPoint(3.18779756, -0.05349352)
SketchPoint_34.setAuxiliary(True)

### Create SketchPoint
SketchPoint_35 = Sketch_1.addPoint(3.04963166, -0.09777598)
SketchPoint_35.setAuxiliary(True)

### Create SketchPoint
SketchPoint_36 = Sketch_1.addPoint(2.89173328, -0.15192468)
SketchPoint_36.setAuxiliary(True)

### Create SketchPoint
SketchPoint_37 = Sketch_1.addPoint(2.70913076, -0.22580586)
SketchPoint_37.setAuxiliary(True)

### Create SketchPoint
SketchPoint_38 = Sketch_1.addPoint(2.54629926, -0.2947539200000001)
SketchPoint_38.setAuxiliary(True)

### Create SketchPoint
SketchPoint_39 = Sketch_1.addPoint(2.39822858, -0.3588074)
SketchPoint_39.setAuxiliary(True)

### Create SketchPoint
SketchPoint_40 = Sketch_1.addPoint(2.26992892, -0.41299464)
SketchPoint_40.setAuxiliary(True)

### Create SketchPoint
SketchPoint_41 = Sketch_1.addPoint(2.12189678, -0.47704812)
SketchPoint_41.setAuxiliary(True)

### Create SketchPoint
SketchPoint_42 = Sketch_1.addPoint(1.8998293, -0.5558238799999999)
SketchPoint_42.setAuxiliary(True)

### Create SketchPoint
SketchPoint_43 = Sketch_1.addPoint(1.70246596, -0.6247334)
SketchPoint_43.setAuxiliary(True)

### Create SketchPoint
SketchPoint_44 = Sketch_1.addPoint(1.52479656, -0.6640442000000001)
SketchPoint_44.setAuxiliary(True)

### Create SketchPoint
SketchPoint_45 = Sketch_1.addPoint(1.33236634, -0.68847856)
SketchPoint_45.setAuxiliary(True)

### Create SketchPoint
SketchPoint_46 = Sketch_1.addPoint(1.15223038, -0.7006186600000001)
SketchPoint_46.setAuxiliary(True)

### Create SketchPoint
SketchPoint_47 = Sketch_1.addPoint(0.9819992000000001, -0.69792086)
SketchPoint_47.setAuxiliary(True)

### Create SketchPoint
SketchPoint_48 = Sketch_1.addPoint(0.83392852, -0.6730240200000001)
SketchPoint_48.setAuxiliary(True)

### Create SketchPoint
SketchPoint_49 = Sketch_1.addPoint(0.68589638, -0.63826094)
SketchPoint_49.setAuxiliary(True)

### Create SketchPoint
SketchPoint_50 = Sketch_1.addPoint(0.5403308, -0.59359308)
SketchPoint_50.setAuxiliary(True)

### Create SketchPoint
SketchPoint_51 = Sketch_1.addPoint(0.4144977, -0.52425962)
SketchPoint_51.setAuxiliary(True)

### Create SketchPoint
SketchPoint_52 = Sketch_1.addPoint(0.30593052, -0.4549647)
SketchPoint_52.setAuxiliary(True)

### Create SketchPoint
SketchPoint_53 = Sketch_1.addPoint(0.19243022, -0.37079334)
SketchPoint_53.setAuxiliary(True)

### Create SketchPoint
SketchPoint_54 = Sketch_1.addPoint(0.09866240000000001, -0.26199492)
SketchPoint_54.setAuxiliary(True)

### Create SketchPoint
SketchPoint_55 = Sketch_1.addPoint(0.04439808000000001, -0.17296752)
SketchPoint_55.setAuxiliary(True)

### Create SketchPoint
SketchPoint_56 = Sketch_1.addPoint(0.009866240000000002, -0.08397866)
SketchPoint_56.setAuxiliary(True)

### Create SketchBSpline
SketchBSpline_1_poles = [(0, 0),
                         (9.014752251795682e-05, 0.05451214529317949),
                         (0.02532271171862411, 0.15579971775632),
                         (0.1083431350565314, 0.2748971692083296),
                         (0.2055162110916063, 0.3762476714704591),
                         (0.3226869043516477, 0.4417758873955893),
                         (0.4396106030030269, 0.5114950821406695),
                         (0.5686113665746824, 0.5543315380381274),
                         (0.7080109281378321, 0.6032643908580265),
                         (0.8627418437966349, 0.6310529967664337),
                         (1.038760710497959, 0.658153841986185),
                         (1.22044861142381, 0.6481760900098785),
                         (1.409221724680022, 0.652722089409154),
                         (1.576578009046891, 0.6165884029638471),
                         (1.750622207371201, 0.6101568179490738),
                         (1.923498511038718, 0.5633997285939256),
                         (2.101468637706691, 0.5160168977121142),
                         (2.26756869845679, 0.4799182653430439),
                         (2.415109663038519, 0.4380247760586003),
                         (2.559003214216268, 0.3937264274604114),
                         (2.717970834232034, 0.3510185960290838),
                         (2.881379255194576, 0.2994130045705585),
                         (3.044331614256414, 0.2554589350745441),
                         (3.196663224032949, 0.2002205281261157),
                         (3.35069087314937, 0.1557508658621287),
                         (3.498630901739528, 0.1100749985226144),
                         (3.626758629069007, 0.07354615416618171),
                         (3.739427723854896, 0.03597634464461345),
                         (3.813716543275791, 0.02837078095396758),
                         (3.876876038873482, -0.02879298081488075),
                         (3.682323148493558, 0.01775150116288641),
                         (3.59938700895519, 0.01042701260276316),
                         (3.435447203044759, 0.007149079513330239),
                         (3.314513764134776, -0.02220772128681934),
                         (3.182869424552179, -0.05232760411385898),
                         (3.042896269666394, -0.100514554949581),
                         (2.881422363667536, -0.1529357624091178),
                         (2.715356347244389, -0.2233467064371046),
                         (2.550824101050389, -0.2921770145749832),
                         (2.405271709687486, -0.3565989754466863),
                         (2.262755596020973, -0.4148284616349733),
                         (2.100460548570712, -0.490254433165252),
                         (1.907228846355101, -0.5500679945331017),
                         (1.712805296160232, -0.627643180207678),
                         (1.521165382362285, -0.66728412705947),
                         (1.336949022926033, -0.6896647036620358),
                         (1.155869424658838, -0.7022133877004375),
                         (0.9881820016357848, -0.7038771014232653),
                         (0.8328845467272493, -0.6739914151454918),
                         (0.6860350214810707, -0.638614040735098),
                         (0.5400698242489836, -0.6015646418675653),
                         (0.4182804086275665, -0.5270754227323547),
                         (0.302266907807943, -0.4533758717656259),
                         (0.1873191972961234, -0.3753781603200299),
                         (0.1039872761267704, -0.2741082432964857),
                         (0.04328037620103962, -0.1770551690563165),
                         (0.006988972040230026, -0.08835445483623842),
                         (-0.0008115069204040052, -0.02871741534942534),
                         (0, 0)
                        ]
SketchBSpline_1 = Sketch_1.addSpline(degree = 3, poles = SketchBSpline_1_poles, weights = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], knots = [0, 0.01841877717868692, 0.03640857044229438, 0.0503319019589481, 0.06751503486217877, 0.08331688612829682, 0.09815550484724707, 0.1151806897007172, 0.1352802264972665, 0.1533760897133781, 0.1776976203324573, 0.1991289647725509, 0.2196078542088847, 0.2376437370925934, 0.2601966555045284, 0.2823272796231457, 0.3025579520551492, 0.3199893311119836, 0.3363024715699076, 0.3555578894595023, 0.3779215054846461, 0.3966205779803185, 0.4149403217914447, 0.43491563393835, 0.453062794308183, 0.4693759347661071, 0.4820291845192418, 0.4942387027706504, 0.496746319874324, 0.5066094106685624, 0.5312792857910991, 0.5454960476108249, 0.5602561299909423, 0.5757710096568369, 0.592796194510307, 0.6123837212555427, 0.6354982845523691, 0.6562477832463178, 0.6751789135618669, 0.6915216952124962, 0.7104486749233012, 0.7380977836510125, 0.762628078574017, 0.7839805979515587, 0.8067422887543781, 0.8279279725980087, 0.8479059568735401, 0.8655249471990413, 0.8833680589533851, 0.9012352967057089, 0.9180940098763235, 0.9332074432767975, 0.9497886416105058, 0.9666426179720728, 0.9788770179440802, 0.9900778977399243, 1], multiplicities = [4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4])
Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchBSpline_1.startPoint())
Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchBSpline_1.endPoint())
Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_3.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_4.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_5.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_6.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_7.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_8.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_9.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_10.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_11.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_12.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_13.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_14.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_15.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_16.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_17.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_18.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_19.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_20.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_21.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_22.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_23.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_24.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_25.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_26.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_27.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_28.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_29.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_30.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_31.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_32.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_33.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_34.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_35.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_36.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_37.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_38.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_39.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_40.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_41.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_42.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_43.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_44.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_45.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_46.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_47.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_48.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_49.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_50.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_51.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_52.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_53.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_54.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_55.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_56.coordinates(), SketchBSpline_1.result())
model.do()


#=========================================================================
# Make offset for testing B-spline
#=========================================================================

# This B-spline still failes on larger values and even smaller in reversed mode.
# But we still consider this as an impruvement because without approximation 
# we have only "Offset algorithm failed" with the same conditions.

# Common offset params
SketchOffset_1_objects = [ SketchBSpline_1 ]
jointType = "KeepDistance"
isApproximated = True

# Test outside offset
offsetValue = 10
isReversed = False

SketchOffset_1 = Sketch_1.addOffset(SketchOffset_1_objects, offsetValue, isReversed, jointType, isApproximated)
model.do()

# Hardcoded value was obtained by running the same script from SALOME Python Console.
assert(len(SketchOffset_1.offset()) == 124)

# Test reversed offset
offsetValue = 0.5
isReversed = True

SketchOffset_2 = Sketch_1.addOffset(SketchOffset_1_objects, offsetValue, isReversed, jointType, isApproximated)
model.do()

assert(len(SketchOffset_2.offset()) == 101)

model.end()

