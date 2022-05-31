# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 36.35, False)

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

### Create SketchPoint
SketchPoint_1 = Sketch_1.addPoint(3.502, 0)
SketchPoint_1.setAuxiliary(True)

### Create SketchPoint
SketchPoint_2 = Sketch_1.addPoint(3.47587508, 0.08247210000000001)
SketchPoint_2.setAuxiliary(True)

### Create SketchPoint
SketchPoint_3 = Sketch_1.addPoint(3.4170765, 0.14106056)
SketchPoint_3.setAuxiliary(True)

### Create SketchPoint
SketchPoint_4 = Sketch_1.addPoint(3.33866672, 0.1866566)
SketchPoint_4.setAuxiliary(True)

### Create SketchPoint
SketchPoint_5 = Sketch_1.addPoint(3.23630326, 0.2409376)
SketchPoint_5.setAuxiliary(True)

### Create SketchPoint
SketchPoint_6 = Sketch_1.addPoint(3.12742608, 0.28649862)
SketchPoint_6.setAuxiliary(True)

### Create SketchPoint
SketchPoint_7 = Sketch_1.addPoint(3.03161136, 0.32123846)
SketchPoint_7.setAuxiliary(True)

### Create SketchPoint
SketchPoint_8 = Sketch_1.addPoint(2.92269916, 0.35163582)
SketchPoint_8.setAuxiliary(True)

### Create SketchPoint
SketchPoint_9 = Sketch_1.addPoint(2.85300936, 0.36900574)
SketchPoint_9.setAuxiliary(True)

### Create SketchPoint
SketchPoint_10 = Sketch_1.addPoint(2.76153712, 0.38637566)
SketchPoint_10.setAuxiliary(True)

### Create SketchPoint
SketchPoint_11 = Sketch_1.addPoint(2.67006488, 0.39936808)
SketchPoint_11.setAuxiliary(True)

### Create SketchPoint
SketchPoint_12 = Sketch_1.addPoint(2.58297014, 0.4167379999999999)
SketchPoint_12.setAuxiliary(True)

### Create SketchPoint
SketchPoint_13 = Sketch_1.addPoint(2.47843544, 0.4297654399999999)
SketchPoint_13.setAuxiliary(True)

### Create SketchPoint
SketchPoint_14 = Sketch_1.addPoint(2.3521183, 0.4384504)
SketchPoint_14.setAuxiliary(True)

### Create SketchPoint
SketchPoint_15 = Sketch_1.addPoint(2.2127387, 0.4384504)
SketchPoint_15.setAuxiliary(True)

### Create SketchPoint
SketchPoint_16 = Sketch_1.addPoint(2.06026162, 0.4384504)
SketchPoint_16.setAuxiliary(True)

### Create SketchPoint
SketchPoint_17 = Sketch_1.addPoint(1.92743076, 0.4275942)
SketchPoint_17.setAuxiliary(True)

### Create SketchPoint
SketchPoint_18 = Sketch_1.addPoint(1.80762734, 0.4167379999999999)
SketchPoint_18.setAuxiliary(True)

### Create SketchPoint
SketchPoint_19 = Sketch_1.addPoint(1.70747014, 0.39936808)
SketchPoint_19.setAuxiliary(True)

### Create SketchPoint
SketchPoint_20 = Sketch_1.addPoint(1.60293544, 0.38203318)
SketchPoint_20.setAuxiliary(True)

### Create SketchPoint
SketchPoint_21 = Sketch_1.addPoint(1.48968076, 0.36466326)
SketchPoint_21.setAuxiliary(True)

### Create SketchPoint
SketchPoint_22 = Sketch_1.addPoint(1.3720836, 0.34295086)
SketchPoint_22.setAuxiliary(True)

### Create SketchPoint
SketchPoint_23 = Sketch_1.addPoint(1.25010894, 0.31689598)
SketchPoint_23.setAuxiliary(True)

### Create SketchPoint
SketchPoint_24 = Sketch_1.addPoint(1.12162056, 0.2908411)
SketchPoint_24.setAuxiliary(True)

### Create SketchPoint
SketchPoint_25 = Sketch_1.addPoint(0.99747466, 0.26047876)
SketchPoint_25.setAuxiliary(True)

### Create SketchPoint
SketchPoint_26 = Sketch_1.addPoint(0.8929399600000001, 0.23442388)
SketchPoint_26.setAuxiliary(True)

### Create SketchPoint
SketchPoint_27 = Sketch_1.addPoint(0.7971252400000002, 0.21271148)
SketchPoint_27.setAuxiliary(True)

### Create SketchPoint
SketchPoint_28 = Sketch_1.addPoint(0.6860417999999999, 0.1866566)
SketchPoint_28.setAuxiliary(True)

### Create SketchPoint
SketchPoint_29 = Sketch_1.addPoint(0.5597246600000001, 0.15625924)
SketchPoint_29.setAuxiliary(True)

### Create SketchPoint
SketchPoint_30 = Sketch_1.addPoint(0.4312362800000002, 0.1258969)
SketchPoint_30.setAuxiliary(True)

### Create SketchPoint
SketchPoint_31 = Sketch_1.addPoint(0.3397640399999999, 0.10201326)
SketchPoint_31.setAuxiliary(True)

### Create SketchPoint
SketchPoint_32 = Sketch_1.addPoint(0.2395718200000001, 0.07812962)
SketchPoint_32.setAuxiliary(True)

### Create SketchPoint
SketchPoint_33 = Sketch_1.addPoint(0.1437571000000002, 0.056032)
SketchPoint_33.setAuxiliary(True)

### Create SketchPoint
SketchPoint_34 = Sketch_1.addPoint(0.06534731999999988, 0.03470482)
SketchPoint_34.setAuxiliary(True)

### Create SketchPoint
SketchPoint_35 = Sketch_1.addPoint(0, 0.003502)
SketchPoint_35.setAuxiliary(True)

### Create SketchPoint
SketchPoint_36 = Sketch_1.addPoint(0.0697248199999998, 0.014008)
SketchPoint_36.setAuxiliary(True)

### Create SketchPoint
SketchPoint_37 = Sketch_1.addPoint(0.17443462, 0.0252144)
SketchPoint_37.setAuxiliary(True)

### Create SketchPoint
SketchPoint_38 = Sketch_1.addPoint(0.25701178, 0.0266152)
SketchPoint_38.setAuxiliary(True)

### Create SketchPoint
SketchPoint_39 = Sketch_1.addPoint(0.3658889599999999, 0.02601986)
SketchPoint_39.setAuxiliary(True)

### Create SketchPoint
SketchPoint_40 = Sketch_1.addPoint(0.4791436400000002, 0.02601986)
SketchPoint_40.setAuxiliary(True)

### Create SketchPoint
SketchPoint_41 = Sketch_1.addPoint(0.5815071000000001, 0.0173349)
SketchPoint_41.setAuxiliary(True)

### Create SketchPoint
SketchPoint_42 = Sketch_1.addPoint(0.7012754999999999, 0.00213622)
SketchPoint_42.setAuxiliary(True)

### Create SketchPoint
SketchPoint_43 = Sketch_1.addPoint(0.8058101999999998, -0.01516366)
SketchPoint_43.setAuxiliary(True)

### Create SketchPoint
SketchPoint_44 = Sketch_1.addPoint(0.9060024200000001, -0.03470482)
SketchPoint_44.setAuxiliary(True)

### Create SketchPoint
SketchPoint_45 = Sketch_1.addPoint(1.03666204, -0.0607597)
SketchPoint_45.setAuxiliary(True)

### Create SketchPoint
SketchPoint_46 = Sketch_1.addPoint(1.14557424, -0.09115706)
SketchPoint_46.setAuxiliary(True)

### Create SketchPoint
SketchPoint_47 = Sketch_1.addPoint(1.2980163, -0.1258969)
SketchPoint_47.setAuxiliary(True)

### Create SketchPoint
SketchPoint_48 = Sketch_1.addPoint(1.4373959, -0.15625924)
SketchPoint_48.setAuxiliary(True)

### Create SketchPoint
SketchPoint_49 = Sketch_1.addPoint(1.55937056, -0.1866566)
SketchPoint_49.setAuxiliary(True)

### Create SketchPoint
SketchPoint_50 = Sketch_1.addPoint(1.6856877, -0.21705396)
SketchPoint_50.setAuxiliary(True)

### Create SketchPoint
SketchPoint_51 = Sketch_1.addPoint(1.81634732, -0.2409376)
SketchPoint_51.setAuxiliary(True)

### Create SketchPoint
SketchPoint_52 = Sketch_1.addPoint(1.94266446, -0.26047876)
SketchPoint_52.setAuxiliary(True)

### Create SketchPoint
SketchPoint_53 = Sketch_1.addPoint(2.08204406, -0.27781366)
SketchPoint_53.setAuxiliary(True)

### Create SketchPoint
SketchPoint_54 = Sketch_1.addPoint(2.22580116, -0.2908411)
SketchPoint_54.setAuxiliary(True)

### Create SketchPoint
SketchPoint_55 = Sketch_1.addPoint(2.36080326, -0.29735482)
SketchPoint_55.setAuxiliary(True)

### Create SketchPoint
SketchPoint_56 = Sketch_1.addPoint(2.51328034, -0.29952606)
SketchPoint_56.setAuxiliary(True)

### Create SketchPoint
SketchPoint_57 = Sketch_1.addPoint(2.63959748, -0.29301234)
SketchPoint_57.setAuxiliary(True)

### Create SketchPoint
SketchPoint_58 = Sketch_1.addPoint(2.77680584, -0.28215614)
SketchPoint_58.setAuxiliary(True)

### Create SketchPoint
SketchPoint_59 = Sketch_1.addPoint(2.89223176, -0.2691287)
SketchPoint_59.setAuxiliary(True)

### Create SketchPoint
SketchPoint_60 = Sketch_1.addPoint(3.0185489, -0.24745132)
SketchPoint_60.setAuxiliary(True)

### Create SketchPoint
SketchPoint_61 = Sketch_1.addPoint(3.16444222, -0.21271148)
SketchPoint_61.setAuxiliary(True)

### Create SketchPoint
SketchPoint_62 = Sketch_1.addPoint(3.31471304, -0.16060172)
SketchPoint_62.setAuxiliary(True)

### Create SketchPoint
SketchPoint_63 = Sketch_1.addPoint(3.40618528, -0.11721194)
SketchPoint_63.setAuxiliary(True)

### Create SketchPoint
SketchPoint_64 = Sketch_1.addPoint(3.4715326, -0.06510218)
SketchPoint_64.setAuxiliary(True)

### Create SketchBSpline
SketchBSpline_1_poles = [(3.502, 0),
                         (3.501708060470513, 0.03160465459957851),
                         (3.482306354808988, 0.08478594846732339),
                         (3.416982460484224, 0.1463028723964636),
                         (3.330867828938158, 0.1897725095120994),
                         (3.23691571256371, 0.2438885707817408),
                         (3.132475493449838, 0.2847168793431304),
                         (3.028693716669709, 0.3242771718865035),
                         (2.935991590725414, 0.3481773438606656),
                         (2.846302117977083, 0.3714272918230952),
                         (2.761971777722034, 0.3876700086132496),
                         (2.670901019132808, 0.3971222319904175),
                         (2.577912212391188, 0.4199064802107029),
                         (2.47138895770733, 0.4307739368475885),
                         (2.347934842807255, 0.4411206976945238),
                         (2.208393417939975, 0.4367094703481399),
                         (2.066490356928406, 0.4424654746602308),
                         (1.931861446733529, 0.4266278861079822),
                         (1.813439066878724, 0.4202402668802049),
                         (1.706149087521655, 0.3984156148279285),
                         (1.600151931908012, 0.381280150658782),
                         (1.487912755840739, 0.3651602028428825),
                         (1.37022484781707, 0.343451145226399),
                         (1.248225848031129, 0.3155391742155946),
                         (1.122361457008797, 0.2925847187390579),
                         (1.003957496481461, 0.2621074537257155),
                         (0.8961617237627484, 0.2345532360663585),
                         (0.7918988749704297, 0.211845235390749),
                         (0.6808742181768952, 0.1856161550032198),
                         (0.5591323563208087, 0.1555837712922512),
                         (0.4432312945768512, 0.1299207001590713),
                         (0.3369105879407892, 0.1005196423732715),
                         (0.2422714917388343, 0.0789561461823229),
                         (0.1454129368472911, 0.05575321928504596),
                         (0.07867537192801388, 0.04292748679362836),
                         (-0.03801510663679483, -0.009036231961024368),
                         (0.09668159902862462, 0.02124137982940557),
                         (0.1631020115172611, 0.02544418940840985),
                         (0.2670052837801755, 0.02725755100255278),
                         (0.3669396449571852, 0.02499900964975237),
                         (0.4758196106561338, 0.02860804255883151),
                         (0.587520466545792, 0.01701112796086315),
                         (0.696501264135322, 0.003600344371230613),
                         (0.8046893603429499, -0.01419519074507264),
                         (0.9160129303585658, -0.03746502686037816),
                         (1.03084989928976, -0.05621611703810876),
                         (1.15892564877219, -0.09721235004459683),
                         (1.293587814245105, -0.1248468755833264),
                         (1.432121846768275, -0.1540096476220431),
                         (1.560708865933035, -0.187062827568635),
                         (1.686271640551389, -0.2189476512659969),
                         (1.814669465172142, -0.2409413298907317),
                         (1.94672182252948, -0.2618358185518865),
                         (2.083258581034335, -0.2785950467488627),
                         (2.222670155691588, -0.2918735040977562),
                         (2.366549046533922, -0.297866371507273),
                         (2.504656418820893, -0.3018436275465021),
                         (2.643344514150033, -0.2930128256147972),
                         (2.769720467648912, -0.2834650455787363),
                         (2.896386200659248, -0.2699367156705364),
                         (3.025904808388414, -0.2471667438643729),
                         (3.168089816001797, -0.2147033470830538),
                         (3.297598829858346, -0.1692120751505936),
                         (3.4021366076639, -0.1232968436621545),
                         (3.474062839110283, -0.07177094713741811),
                         (3.497525008229238, -0.02562138888775374),
                         (3.502, 0)
                        ]
SketchBSpline_1 = Sketch_1.addSpline(degree = 3, poles = SketchBSpline_1_poles, weights = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], knots = [0, 0.01183488863837179, 0.0231901864984856, 0.03559858893144927, 0.05144916515834002, 0.06759531467622239, 0.0815379341596944, 0.09700677492797639, 0.1068321569610141, 0.1195693666101132, 0.1322085557927937, 0.1443579416201434, 0.1587691279665198, 0.1760903712525299, 0.1951577895815503, 0.2160169703364635, 0.2342490967534491, 0.2507056052742759, 0.2646118431448987, 0.2791077011690486, 0.2947823396709444, 0.3111417859997592, 0.3282046159595691, 0.3461398429174012, 0.3636238095657637, 0.3783618826153202, 0.391801869278653, 0.4074107325669362, 0.4251844860013295, 0.4432460569026663, 0.4561791711061376, 0.4702697244163377, 0.4837214559733177, 0.4948377859291224, 0.5047442665271804, 0.5143904392393579, 0.5287967600422182, 0.5400951120443203, 0.5549899587260146, 0.5704834338205002, 0.5845372797011691, 0.601053244848017, 0.6155483198902286, 0.6295130832805995, 0.6477395111687876, 0.6632083519370694, 0.6845974067707579, 0.7041119934399063, 0.7213087363026549, 0.7390824897370483, 0.7572531654218201, 0.7747391660376208, 0.7939534892917685, 0.8137003452650871, 0.8321903971419141, 0.8530516926114459, 0.8703550992317159, 0.8891841498048808, 0.9050749088751692, 0.9226079659764084, 0.9431244997359293, 0.9648828024586863, 0.9787328512217585, 0.9901668364115462, 1], multiplicities = [4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4])
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
Sketch_1.setCoincident(SketchPoint_57.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_58.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_59.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_60.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_61.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_62.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_63.coordinates(), SketchBSpline_1.result())
Sketch_1.setCoincident(SketchPoint_64.coordinates(), SketchBSpline_1.result())

### Create SketchOffset
SketchOffset_1 = Sketch_1.addOffset([SketchBSpline_1.result()], 0.01, False)
[SketchBSpline_2, SketchBSpline_3, SketchBSpline_4, SketchBSpline_5, SketchBSpline_6, SketchBSpline_7, SketchBSpline_8, SketchBSpline_9, SketchBSpline_10, SketchBSpline_11, SketchBSpline_12, SketchBSpline_13, SketchBSpline_14, SketchBSpline_15, SketchBSpline_16, SketchBSpline_17, SketchBSpline_18, SketchBSpline_19, SketchBSpline_20, SketchBSpline_21, SketchBSpline_22, SketchBSpline_23, SketchBSpline_24, SketchBSpline_25, SketchBSpline_26, SketchBSpline_27, SketchBSpline_28, SketchBSpline_29, SketchBSpline_30, SketchBSpline_31, SketchBSpline_32, SketchBSpline_33, SketchBSpline_34, SketchBSpline_35, SketchBSpline_36, SketchBSpline_37, SketchBSpline_38, SketchBSpline_39, SketchBSpline_40, SketchBSpline_41, SketchBSpline_42, SketchBSpline_43, SketchBSpline_44, SketchBSpline_45, SketchBSpline_46, SketchBSpline_47, SketchBSpline_48, SketchBSpline_49, SketchBSpline_50, SketchBSpline_51, SketchBSpline_52, SketchBSpline_53, SketchBSpline_54, SketchBSpline_55, SketchBSpline_56, SketchBSpline_57, SketchBSpline_58, SketchBSpline_59, SketchBSpline_60, SketchBSpline_61, SketchArc_1] = SketchOffset_1.offset()

### Create SketchOffset
SketchOffset_2 = Sketch_1.addOffset([SketchBSpline_1.result()], 0.07000000000000001, False)
[SketchBSpline_62, SketchBSpline_63, SketchBSpline_64, SketchBSpline_65, SketchBSpline_66, SketchBSpline_67, SketchBSpline_68, SketchBSpline_69, SketchBSpline_70, SketchBSpline_71, SketchBSpline_72, SketchBSpline_73, SketchBSpline_74, SketchBSpline_75, SketchBSpline_76, SketchBSpline_77, SketchBSpline_78, SketchBSpline_79, SketchBSpline_80, SketchBSpline_81, SketchBSpline_82, SketchBSpline_83, SketchBSpline_84, SketchBSpline_85, SketchBSpline_86, SketchBSpline_87, SketchBSpline_88, SketchBSpline_89, SketchBSpline_90, SketchBSpline_91, SketchBSpline_92, SketchBSpline_93, SketchBSpline_94, SketchBSpline_95, SketchBSpline_96, SketchBSpline_97, SketchBSpline_98, SketchBSpline_99, SketchBSpline_100, SketchBSpline_101, SketchBSpline_102, SketchBSpline_103, SketchBSpline_104, SketchBSpline_105, SketchBSpline_106, SketchBSpline_107, SketchBSpline_108, SketchBSpline_109, SketchBSpline_110, SketchBSpline_111, SketchBSpline_112, SketchBSpline_113, SketchBSpline_114, SketchBSpline_115, SketchBSpline_116, SketchBSpline_117, SketchBSpline_118, SketchBSpline_119, SketchBSpline_120, SketchBSpline_121, SketchArc_2] = SketchOffset_2.offset()
model.do()

### Create Wire
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "SketchBSpline_1")], False)

### Create Wire
Wire_2_objects = [model.selection("EDGE", "SketchBSpline_2"),
                  model.selection("EDGE", "SketchBSpline_3"),
                  model.selection("EDGE", "SketchBSpline_4"),
                  model.selection("EDGE", "SketchBSpline_5"),
                  model.selection("EDGE", "SketchBSpline_6"),
                  model.selection("EDGE", "SketchBSpline_7"),
                  model.selection("EDGE", "SketchBSpline_8"),
                  model.selection("EDGE", "SketchBSpline_9"),
                  model.selection("EDGE", "SketchBSpline_10"),
                  model.selection("EDGE", "SketchBSpline_11"),
                  model.selection("EDGE", "SketchBSpline_12"),
                  model.selection("EDGE", "SketchBSpline_13"),
                  model.selection("EDGE", "SketchBSpline_14"),
                  model.selection("EDGE", "SketchBSpline_15"),
                  model.selection("EDGE", "SketchBSpline_16"),
                  model.selection("EDGE", "SketchBSpline_17"),
                  model.selection("EDGE", "SketchBSpline_18"),
                  model.selection("EDGE", "SketchBSpline_19"),
                  model.selection("EDGE", "SketchBSpline_20"),
                  model.selection("EDGE", "SketchBSpline_21"),
                  model.selection("EDGE", "SketchBSpline_22"),
                  model.selection("EDGE", "SketchBSpline_23"),
                  model.selection("EDGE", "SketchBSpline_24"),
                  model.selection("EDGE", "SketchBSpline_25"),
                  model.selection("EDGE", "SketchBSpline_26"),
                  model.selection("EDGE", "SketchBSpline_27"),
                  model.selection("EDGE", "SketchBSpline_28"),
                  model.selection("EDGE", "SketchBSpline_29"),
                  model.selection("EDGE", "SketchBSpline_30"),
                  model.selection("EDGE", "SketchBSpline_31"),
                  model.selection("EDGE", "SketchBSpline_32"),
                  model.selection("EDGE", "SketchBSpline_33"),
                  model.selection("EDGE", "SketchBSpline_34"),
                  model.selection("EDGE", "SketchBSpline_35"),
                  model.selection("EDGE", "SketchBSpline_36"),
                  model.selection("EDGE", "SketchBSpline_37"),
                  model.selection("EDGE", "SketchBSpline_38"),
                  model.selection("EDGE", "SketchBSpline_39"),
                  model.selection("EDGE", "SketchBSpline_40"),
                  model.selection("EDGE", "SketchBSpline_41"),
                  model.selection("EDGE", "SketchBSpline_42"),
                  model.selection("EDGE", "SketchBSpline_43"),
                  model.selection("EDGE", "SketchBSpline_44"),
                  model.selection("EDGE", "SketchBSpline_45"),
                  model.selection("EDGE", "SketchBSpline_46"),
                  model.selection("EDGE", "SketchBSpline_47"),
                  model.selection("EDGE", "SketchBSpline_48"),
                  model.selection("EDGE", "SketchBSpline_49"),
                  model.selection("EDGE", "SketchBSpline_50"),
                  model.selection("EDGE", "SketchBSpline_51"),
                  model.selection("EDGE", "SketchBSpline_52"),
                  model.selection("EDGE", "SketchBSpline_53"),
                  model.selection("EDGE", "SketchBSpline_54"),
                  model.selection("EDGE", "SketchBSpline_55"),
                  model.selection("EDGE", "SketchBSpline_56"),
                  model.selection("EDGE", "SketchBSpline_57"),
                  model.selection("EDGE", "SketchBSpline_58"),
                  model.selection("EDGE", "SketchBSpline_59"),
                  model.selection("EDGE", "SketchBSpline_60"),
                  model.selection("EDGE", "SketchBSpline_61"),
                  model.selection("EDGE", "SketchArc_1_2")]
Wire_2 = model.addWire(Part_1_doc, Wire_2_objects, False)

### Create Wire
Wire_3_objects = [model.selection("EDGE", "SketchBSpline_62"),
                  model.selection("EDGE", "SketchBSpline_63"),
                  model.selection("EDGE", "SketchBSpline_64"),
                  model.selection("EDGE", "SketchBSpline_65"),
                  model.selection("EDGE", "SketchBSpline_66"),
                  model.selection("EDGE", "SketchBSpline_67"),
                  model.selection("EDGE", "SketchBSpline_68"),
                  model.selection("EDGE", "SketchBSpline_69"),
                  model.selection("EDGE", "SketchBSpline_70"),
                  model.selection("EDGE", "SketchBSpline_71"),
                  model.selection("EDGE", "SketchBSpline_72"),
                  model.selection("EDGE", "SketchBSpline_73"),
                  model.selection("EDGE", "SketchBSpline_74"),
                  model.selection("EDGE", "SketchBSpline_75"),
                  model.selection("EDGE", "SketchBSpline_76"),
                  model.selection("EDGE", "SketchBSpline_77"),
                  model.selection("EDGE", "SketchBSpline_78"),
                  model.selection("EDGE", "SketchBSpline_79"),
                  model.selection("EDGE", "SketchBSpline_80"),
                  model.selection("EDGE", "SketchBSpline_81"),
                  model.selection("EDGE", "SketchBSpline_82"),
                  model.selection("EDGE", "SketchBSpline_83"),
                  model.selection("EDGE", "SketchBSpline_84"),
                  model.selection("EDGE", "SketchBSpline_85"),
                  model.selection("EDGE", "SketchBSpline_86"),
                  model.selection("EDGE", "SketchBSpline_87"),
                  model.selection("EDGE", "SketchBSpline_88"),
                  model.selection("EDGE", "SketchBSpline_89"),
                  model.selection("EDGE", "SketchBSpline_90"),
                  model.selection("EDGE", "SketchBSpline_91"),
                  model.selection("EDGE", "SketchBSpline_92"),
                  model.selection("EDGE", "SketchBSpline_93"),
                  model.selection("EDGE", "SketchBSpline_94"),
                  model.selection("EDGE", "SketchBSpline_95"),
                  model.selection("EDGE", "SketchBSpline_96"),
                  model.selection("EDGE", "SketchBSpline_97"),
                  model.selection("EDGE", "SketchBSpline_98"),
                  model.selection("EDGE", "SketchBSpline_99"),
                  model.selection("EDGE", "SketchBSpline_100"),
                  model.selection("EDGE", "SketchBSpline_101"),
                  model.selection("EDGE", "SketchBSpline_102"),
                  model.selection("EDGE", "SketchBSpline_103"),
                  model.selection("EDGE", "SketchBSpline_104"),
                  model.selection("EDGE", "SketchBSpline_105"),
                  model.selection("EDGE", "SketchBSpline_106"),
                  model.selection("EDGE", "SketchBSpline_107"),
                  model.selection("EDGE", "SketchBSpline_108"),
                  model.selection("EDGE", "SketchBSpline_109"),
                  model.selection("EDGE", "SketchBSpline_110"),
                  model.selection("EDGE", "SketchBSpline_111"),
                  model.selection("EDGE", "SketchBSpline_112"),
                  model.selection("EDGE", "SketchBSpline_113"),
                  model.selection("EDGE", "SketchBSpline_114"),
                  model.selection("EDGE", "SketchBSpline_115"),
                  model.selection("EDGE", "SketchBSpline_116"),
                  model.selection("EDGE", "SketchBSpline_117"),
                  model.selection("EDGE", "SketchBSpline_118"),
                  model.selection("EDGE", "SketchBSpline_119"),
                  model.selection("EDGE", "SketchBSpline_120"),
                  model.selection("EDGE", "SketchBSpline_121"),
                  model.selection("EDGE", "SketchArc_2_2")]
Wire_3 = model.addWire(Part_1_doc, Wire_3_objects, False)

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 40.45, False)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))

### Create SketchPoint
SketchPoint_65 = Sketch_2.addPoint(3.256, 0)
SketchPoint_65.setAuxiliary(True)

### Create SketchPoint
SketchPoint_66 = Sketch_2.addPoint(3.23171024, 0.07667880000000001)
SketchPoint_66.setAuxiliary(True)

### Create SketchPoint
SketchPoint_67 = Sketch_2.addPoint(3.177042, 0.13115168)
SketchPoint_67.setAuxiliary(True)

### Create SketchPoint
SketchPoint_68 = Sketch_2.addPoint(3.10414016, 0.1735448)
SketchPoint_68.setAuxiliary(True)

### Create SketchPoint
SketchPoint_69 = Sketch_2.addPoint(3.00896728, 0.2240128)
SketchPoint_69.setAuxiliary(True)

### Create SketchPoint
SketchPoint_70 = Sketch_2.addPoint(2.90773824, 0.2663733599999999)
SketchPoint_70.setAuxiliary(True)

### Create SketchPoint
SketchPoint_71 = Sketch_2.addPoint(2.81865408, 0.29867288)
SketchPoint_71.setAuxiliary(True)

### Create SketchPoint
SketchPoint_72 = Sketch_2.addPoint(2.71739248, 0.32693496)
SketchPoint_72.setAuxiliary(True)

### Create SketchPoint
SketchPoint_73 = Sketch_2.addPoint(2.65259808, 0.34308472)
SketchPoint_73.setAuxiliary(True)

### Create SketchPoint
SketchPoint_74 = Sketch_2.addPoint(2.56755136, 0.35923448)
SketchPoint_74.setAuxiliary(True)

### Create SketchPoint
SketchPoint_75 = Sketch_2.addPoint(2.48250464, 0.37131424)
SketchPoint_75.setAuxiliary(True)

### Create SketchPoint
SketchPoint_76 = Sketch_2.addPoint(2.40152792, 0.387464)
SketchPoint_76.setAuxiliary(True)

### Create SketchPoint
SketchPoint_77 = Sketch_2.addPoint(2.30433632, 0.39957632)
SketchPoint_77.setAuxiliary(True)

### Create SketchPoint
SketchPoint_78 = Sketch_2.addPoint(2.1868924, 0.4076512)
SketchPoint_78.setAuxiliary(True)

### Create SketchPoint
SketchPoint_79 = Sketch_2.addPoint(2.0573036, 0.4076512)
SketchPoint_79.setAuxiliary(True)

### Create SketchPoint
SketchPoint_80 = Sketch_2.addPoint(1.91553736, 0.4076512)
SketchPoint_80.setAuxiliary(True)

### Create SketchPoint
SketchPoint_81 = Sketch_2.addPoint(1.79203728, 0.3975576)
SketchPoint_81.setAuxiliary(True)

### Create SketchPoint
SketchPoint_82 = Sketch_2.addPoint(1.68064952, 0.387464)
SketchPoint_82.setAuxiliary(True)

### Create SketchPoint
SketchPoint_83 = Sketch_2.addPoint(1.58752792, 0.37131424)
SketchPoint_83.setAuxiliary(True)

### Create SketchPoint
SketchPoint_84 = Sketch_2.addPoint(1.49033632, 0.35519704)
SketchPoint_84.setAuxiliary(True)

### Create SketchPoint
SketchPoint_85 = Sketch_2.addPoint(1.38503728, 0.33904728)
SketchPoint_85.setAuxiliary(True)

### Create SketchPoint
SketchPoint_86 = Sketch_2.addPoint(1.2757008, 0.31886008)
SketchPoint_86.setAuxiliary(True)

### Create SketchPoint
SketchPoint_87 = Sketch_2.addPoint(1.16229432, 0.29463544)
SketchPoint_87.setAuxiliary(True)

### Create SketchPoint
SketchPoint_88 = Sketch_2.addPoint(1.04283168, 0.2704108)
SketchPoint_88.setAuxiliary(True)

### Create SketchPoint
SketchPoint_89 = Sketch_2.addPoint(0.9274064800000001, 0.24218128)
SketchPoint_89.setAuxiliary(True)

### Create SketchPoint
SketchPoint_90 = Sketch_2.addPoint(0.8302148799999998, 0.21795664)
SketchPoint_90.setAuxiliary(True)

### Create SketchPoint
SketchPoint_91 = Sketch_2.addPoint(0.7411307200000001, 0.19776944)
SketchPoint_91.setAuxiliary(True)

### Create SketchPoint
SketchPoint_92 = Sketch_2.addPoint(0.6378504, 0.1735448)
SketchPoint_92.setAuxiliary(True)

### Create SketchPoint
SketchPoint_93 = Sketch_2.addPoint(0.5204064800000001, 0.14528272)
SketchPoint_93.setAuxiliary(True)

### Create SketchPoint
SketchPoint_94 = Sketch_2.addPoint(0.4009438400000001, 0.1170532)
SketchPoint_94.setAuxiliary(True)

### Create SketchPoint
SketchPoint_95 = Sketch_2.addPoint(0.3158971199999998, 0.09484727999999999)
SketchPoint_95.setAuxiliary(True)

### Create SketchPoint
SketchPoint_96 = Sketch_2.addPoint(0.2227429599999997, 0.07264135999999999)
SketchPoint_96.setAuxiliary(True)

### Create SketchPoint
SketchPoint_97 = Sketch_2.addPoint(0.1336588000000001, 0.052096)
SketchPoint_97.setAuxiliary(True)

### Create SketchPoint
SketchPoint_98 = Sketch_2.addPoint(0.06075695999999997, 0.03226696)
SketchPoint_98.setAuxiliary(True)

### Create SketchPoint
SketchPoint_99 = Sketch_2.addPoint(0, 0.003256)
SketchPoint_99.setAuxiliary(True)

### Create SketchPoint
SketchPoint_100 = Sketch_2.addPoint(0.06482695999999999, 0.013024)
SketchPoint_100.setAuxiliary(True)

### Create SketchPoint
SketchPoint_101 = Sketch_2.addPoint(0.1621813599999999, 0.0234432)
SketchPoint_101.setAuxiliary(True)

### Create SketchPoint
SketchPoint_102 = Sketch_2.addPoint(0.2389578399999999, 0.0247456)
SketchPoint_102.setAuxiliary(True)

### Create SketchPoint
SketchPoint_103 = Sketch_2.addPoint(0.3401868800000001, 0.02419208)
SketchPoint_103.setAuxiliary(True)

### Create SketchPoint
SketchPoint_104 = Sketch_2.addPoint(0.4454859200000003, 0.02419208)
SketchPoint_104.setAuxiliary(True)

### Create SketchPoint
SketchPoint_105 = Sketch_2.addPoint(0.5406588000000001, 0.0161172)
SketchPoint_105.setAuxiliary(True)

### Create SketchPoint
SketchPoint_106 = Sketch_2.addPoint(0.6520139999999999, 0.00198616)
SketchPoint_106.setAuxiliary(True)

### Create SketchPoint
SketchPoint_107 = Sketch_2.addPoint(0.7492055999999998, -0.01409848)
SketchPoint_107.setAuxiliary(True)

### Create SketchPoint
SketchPoint_108 = Sketch_2.addPoint(0.8423597599999999, -0.03226696)
SketchPoint_108.setAuxiliary(True)

### Create SketchPoint
SketchPoint_109 = Sketch_2.addPoint(0.9638411199999997, -0.0564916)
SketchPoint_109.setAuxiliary(True)

### Create SketchPoint
SketchPoint_110 = Sketch_2.addPoint(1.06510272, -0.08475368)
SketchPoint_110.setAuxiliary(True)

### Create SketchPoint
SketchPoint_111 = Sketch_2.addPoint(1.2068364, -0.1170532)
SketchPoint_111.setAuxiliary(True)

### Create SketchPoint
SketchPoint_112 = Sketch_2.addPoint(1.3364252, -0.14528272)
SketchPoint_112.setAuxiliary(True)

### Create SketchPoint
SketchPoint_113 = Sketch_2.addPoint(1.44983168, -0.1735448)
SketchPoint_113.setAuxiliary(True)

### Create SketchPoint
SketchPoint_114 = Sketch_2.addPoint(1.5672756, -0.20180688)
SketchPoint_114.setAuxiliary(True)

### Create SketchPoint
SketchPoint_115 = Sketch_2.addPoint(1.68875696, -0.2240128)
SketchPoint_115.setAuxiliary(True)

### Create SketchPoint
SketchPoint_116 = Sketch_2.addPoint(1.80620088, -0.24218128)
SketchPoint_116.setAuxiliary(True)

### Create SketchPoint
SketchPoint_117 = Sketch_2.addPoint(1.93578968, -0.25829848)
SketchPoint_117.setAuxiliary(True)

### Create SketchPoint
SketchPoint_118 = Sketch_2.addPoint(2.06944848, -0.2704108)
SketchPoint_118.setAuxiliary(True)

### Create SketchPoint
SketchPoint_119 = Sketch_2.addPoint(2.19496728, -0.27646696)
SketchPoint_119.setAuxiliary(True)

### Create SketchPoint
SketchPoint_120 = Sketch_2.addPoint(2.33673352, -0.27848568)
SketchPoint_120.setAuxiliary(True)

### Create SketchPoint
SketchPoint_121 = Sketch_2.addPoint(2.45417744, -0.27242952)
SketchPoint_121.setAuxiliary(True)

### Create SketchPoint
SketchPoint_122 = Sketch_2.addPoint(2.58174752, -0.26233592)
SketchPoint_122.setAuxiliary(True)

### Create SketchPoint
SketchPoint_123 = Sketch_2.addPoint(2.68906528, -0.2502236)
SketchPoint_123.setAuxiliary(True)

### Create SketchPoint
SketchPoint_124 = Sketch_2.addPoint(2.8065092, -0.23006896)
SketchPoint_124.setAuxiliary(True)

### Create SketchPoint
SketchPoint_125 = Sketch_2.addPoint(2.94215416, -0.19776944)
SketchPoint_125.setAuxiliary(True)

### Create SketchPoint
SketchPoint_126 = Sketch_2.addPoint(3.08186912, -0.14932016)
SketchPoint_126.setAuxiliary(True)

### Create SketchPoint
SketchPoint_127 = Sketch_2.addPoint(3.16691584, -0.10897832)
SketchPoint_127.setAuxiliary(True)

### Create SketchPoint
SketchPoint_128 = Sketch_2.addPoint(3.2276728, -0.06052903999999999)
SketchPoint_128.setAuxiliary(True)

### Create SketchBSpline
SketchBSpline_122_poles = [(3.256, 0),
                           (3.255728567930323, 0.02938456749749511),
                           (3.237689746218751, 0.07883011085368516),
                           (3.176954566343984, 0.1360257431533082),
                           (3.09688910651703, 0.1764418306600214),
                           (3.00953671048185, 0.2267564781454451),
                           (2.912432954503905, 0.2647167787382159),
                           (2.815941388200049, 0.3014981358259439),
                           (2.729751176299813, 0.3237194265020923),
                           (2.646361992042657, 0.3453361685254134),
                           (2.567955484940874, 0.3604379063520104),
                           (2.483282044059514, 0.3692261528728723),
                           (2.396825289419105, 0.3904099084997285),
                           (2.297784821900362, 0.4005139744076952),
                           (2.183002812158886, 0.4101339211003338),
                           (2.05326355477229, 0.406032562950755),
                           (1.921328555727838, 0.4113842334362396),
                           (1.796156730600904, 0.3966591653819502),
                           (1.686052998788443, 0.3907202481330517),
                           (1.586299665611225, 0.3704286812906156),
                           (1.487748341031549, 0.3544969076370629),
                           (1.383393470307667, 0.3395093148076599),
                           (1.273972616931005, 0.319325222403528),
                           (1.160543506907297, 0.2933739438166694),
                           (1.043520532273171, 0.2720319372399694),
                           (0.9334339259119463, 0.2436955652001511),
                           (0.833210329118078, 0.2180769093752324),
                           (0.7362714839816442, 0.1969640452405135),
                           (0.6330458179280327, 0.1725774416591901),
                           (0.5198557830327111, 0.1446546999793175),
                           (0.4120962578932688, 0.1207943460074061),
                           (0.313244110318449, 0.09345858240073433),
                           (0.2252529917480424, 0.07340982637625464),
                           (0.1351983216375724, 0.05183680239637619),
                           (0.07314877527059195, 0.03991202084524661),
                           (-0.03534471365202863, -0.00840147666050696),
                           (0.08989014461370692, 0.01974926691163458),
                           (0.1516448171045691, 0.0236568477195267),
                           (0.2482493443712883, 0.02534282868769613),
                           (0.3411637589893189, 0.02324293986853048),
                           (0.4423953890052463, 0.02659845418947896),
                           (0.5462497541613646, 0.01581617151358379),
                           (0.6475751330738458, 0.003347436114427953),
                           (0.7481634943679742, -0.01319804142374542),
                           (0.8516670763128179, -0.03483327454522876),
                           (0.9584372564498741, -0.05226718363109152),
                           (1.077516251399843, -0.09038361271993345),
                           (1.202718995768721, -0.1160769351511452),
                           (1.331521625664622, -0.1431911515298037),
                           (1.451075975864637, -0.1739224918799187),
                           (1.567818521312199, -0.2035675478361183),
                           (1.687196967047543, -0.2240162678824165),
                           (1.809973230769842, -0.2434430111950148),
                           (1.936918886307194, -0.259024977788206),
                           (2.066537414886297, -0.2713706822793528),
                           (2.200309450461007, -0.2769425772780358),
                           (2.328715391113886, -0.2806404486840122),
                           (2.457661261585523, -0.2724299715025069),
                           (2.575159863696419, -0.2635528807551016),
                           (2.692927889590666, -0.2509748561459927),
                           (2.81334838838169, -0.2298043740783547),
                           (2.945545528527086, -0.1996213872365573),
                           (3.065957107372579, -0.1573256758110601),
                           (3.163151568975917, -0.114635786111929),
                           (3.230025301011729, -0.0667293557622594),
                           (3.251839356594632, -0.02382159971973897),
                           (3.256, 0)
                          ]
SketchBSpline_122 = Sketch_2.addSpline(degree = 3, poles = SketchBSpline_122_poles, weights = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], knots = [0, 0.01183488863837179, 0.02319018649848563, 0.03559858893144933, 0.05144916515834002, 0.06759531467622243, 0.0815379341596944, 0.09700677492797637, 0.1068321569610141, 0.1195693666101132, 0.1322085557927938, 0.1443579416201434, 0.1587691279665199, 0.17609037125253, 0.1951577895815503, 0.2160169703364635, 0.2342490967534491, 0.2507056052742759, 0.2646118431448987, 0.2791077011690486, 0.2947823396709444, 0.3111417859997592, 0.3282046159595691, 0.3461398429174012, 0.3636238095657636, 0.3783618826153202, 0.3918018692786528, 0.4074107325669361, 0.4251844860013295, 0.4432460569026663, 0.4561791711061376, 0.4702697244163377, 0.4837214559733176, 0.4948377859291223, 0.5047442665271803, 0.514390439239358, 0.5287967600422183, 0.5400951120443203, 0.5549899587260146, 0.5704834338205, 0.584537279701169, 0.6010532448480169, 0.6155483198902286, 0.6295130832805996, 0.6477395111687876, 0.6632083519370696, 0.6845974067707581, 0.7041119934399064, 0.721308736302655, 0.7390824897370484, 0.7572531654218201, 0.7747391660376209, 0.7939534892917686, 0.8137003452650872, 0.8321903971419143, 0.8530516926114459, 0.8703550992317159, 0.8891841498048809, 0.9050749088751692, 0.9226079659764083, 0.9431244997359293, 0.9648828024586862, 0.9787328512217585, 0.9901668364115461, 1], multiplicities = [4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4])
Sketch_2.setCoincident(SketchPoint_65.coordinates(), SketchBSpline_122.startPoint())
Sketch_2.setCoincident(SketchPoint_65.coordinates(), SketchBSpline_122.endPoint())
Sketch_2.setCoincident(SketchPoint_66.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_67.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_68.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_69.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_70.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_71.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_72.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_73.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_74.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_75.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_76.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_77.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_78.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_79.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_80.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_81.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_82.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_83.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_84.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_85.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_86.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_87.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_88.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_89.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_90.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_91.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_92.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_93.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_94.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_95.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_96.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_97.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_98.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_99.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_100.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_101.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_102.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_103.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_104.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_105.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_106.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_107.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_108.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_109.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_110.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_111.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_112.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_113.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_114.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_115.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_116.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_117.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_118.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_119.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_120.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_121.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_122.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_123.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_124.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_125.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_126.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_127.coordinates(), SketchBSpline_122.result())
Sketch_2.setCoincident(SketchPoint_128.coordinates(), SketchBSpline_122.result())

### Create SketchOffset
SketchOffset_3 = Sketch_2.addOffset([SketchBSpline_122.result()], 0.01, False)
[SketchBSpline_123, SketchBSpline_124, SketchBSpline_125, SketchBSpline_126, SketchBSpline_127, SketchBSpline_128, SketchBSpline_129, SketchBSpline_130, SketchBSpline_131, SketchBSpline_132, SketchBSpline_133, SketchBSpline_134, SketchBSpline_135, SketchBSpline_136, SketchBSpline_137, SketchBSpline_138, SketchBSpline_139, SketchBSpline_140, SketchBSpline_141, SketchBSpline_142, SketchBSpline_143, SketchBSpline_144, SketchBSpline_145, SketchBSpline_146, SketchBSpline_147, SketchBSpline_148, SketchBSpline_149, SketchBSpline_150, SketchBSpline_151, SketchBSpline_152, SketchBSpline_153, SketchBSpline_154, SketchBSpline_155, SketchBSpline_156, SketchBSpline_157, SketchBSpline_158, SketchBSpline_159, SketchBSpline_160, SketchBSpline_161, SketchBSpline_162, SketchBSpline_163, SketchBSpline_164, SketchBSpline_165, SketchBSpline_166, SketchBSpline_167, SketchBSpline_168, SketchBSpline_169, SketchBSpline_170, SketchBSpline_171, SketchBSpline_172, SketchBSpline_173, SketchBSpline_174, SketchBSpline_175, SketchBSpline_176, SketchBSpline_177, SketchBSpline_178, SketchBSpline_179, SketchBSpline_180, SketchBSpline_181, SketchBSpline_182, SketchArc_3] = SketchOffset_3.offset()

### Create SketchOffset
SketchOffset_4 = Sketch_2.addOffset([SketchBSpline_122.result()], 0.07000000000000001, False)
[SketchBSpline_183, SketchBSpline_184, SketchBSpline_185, SketchBSpline_186, SketchBSpline_187, SketchBSpline_188, SketchBSpline_189, SketchBSpline_190, SketchBSpline_191, SketchBSpline_192, SketchBSpline_193, SketchBSpline_194, SketchBSpline_195, SketchBSpline_196, SketchBSpline_197, SketchBSpline_198, SketchBSpline_199, SketchBSpline_200, SketchBSpline_201, SketchBSpline_202, SketchBSpline_203, SketchBSpline_204, SketchBSpline_205, SketchBSpline_206, SketchBSpline_207, SketchBSpline_208, SketchBSpline_209, SketchBSpline_210, SketchBSpline_211, SketchBSpline_212, SketchBSpline_213, SketchBSpline_214, SketchBSpline_215, SketchBSpline_216, SketchBSpline_217, SketchBSpline_218, SketchBSpline_219, SketchBSpline_220, SketchBSpline_221, SketchBSpline_222, SketchBSpline_223, SketchBSpline_224, SketchBSpline_225, SketchBSpline_226, SketchBSpline_227, SketchBSpline_228, SketchBSpline_229, SketchBSpline_230, SketchBSpline_231, SketchBSpline_232, SketchBSpline_233, SketchBSpline_234, SketchBSpline_235, SketchBSpline_236, SketchBSpline_237, SketchBSpline_238, SketchBSpline_239, SketchBSpline_240, SketchBSpline_241, SketchBSpline_242, SketchArc_4] = SketchOffset_4.offset()
model.do()

### Create Wire
Wire_4 = model.addWire(Part_1_doc, [model.selection("EDGE", "SketchBSpline_122")], False)

### Create Wire
Wire_5_objects = [model.selection("EDGE", "SketchBSpline_123"),
                  model.selection("EDGE", "SketchBSpline_124"),
                  model.selection("EDGE", "SketchBSpline_125"),
                  model.selection("EDGE", "SketchBSpline_126"),
                  model.selection("EDGE", "SketchBSpline_127"),
                  model.selection("EDGE", "SketchBSpline_128"),
                  model.selection("EDGE", "SketchBSpline_129"),
                  model.selection("EDGE", "SketchBSpline_130"),
                  model.selection("EDGE", "SketchBSpline_131"),
                  model.selection("EDGE", "SketchBSpline_132"),
                  model.selection("EDGE", "SketchBSpline_133"),
                  model.selection("EDGE", "SketchBSpline_134"),
                  model.selection("EDGE", "SketchBSpline_135"),
                  model.selection("EDGE", "SketchBSpline_136"),
                  model.selection("EDGE", "SketchBSpline_137"),
                  model.selection("EDGE", "SketchBSpline_138"),
                  model.selection("EDGE", "SketchBSpline_139"),
                  model.selection("EDGE", "SketchBSpline_140"),
                  model.selection("EDGE", "SketchBSpline_141"),
                  model.selection("EDGE", "SketchBSpline_142"),
                  model.selection("EDGE", "SketchBSpline_143"),
                  model.selection("EDGE", "SketchBSpline_144"),
                  model.selection("EDGE", "SketchBSpline_145"),
                  model.selection("EDGE", "SketchBSpline_146"),
                  model.selection("EDGE", "SketchBSpline_147"),
                  model.selection("EDGE", "SketchBSpline_148"),
                  model.selection("EDGE", "SketchBSpline_149"),
                  model.selection("EDGE", "SketchBSpline_150"),
                  model.selection("EDGE", "SketchBSpline_151"),
                  model.selection("EDGE", "SketchBSpline_152"),
                  model.selection("EDGE", "SketchBSpline_153"),
                  model.selection("EDGE", "SketchBSpline_154"),
                  model.selection("EDGE", "SketchBSpline_155"),
                  model.selection("EDGE", "SketchBSpline_156"),
                  model.selection("EDGE", "SketchBSpline_157"),
                  model.selection("EDGE", "SketchBSpline_158"),
                  model.selection("EDGE", "SketchBSpline_159"),
                  model.selection("EDGE", "SketchBSpline_160"),
                  model.selection("EDGE", "SketchBSpline_161"),
                  model.selection("EDGE", "SketchBSpline_162"),
                  model.selection("EDGE", "SketchBSpline_163"),
                  model.selection("EDGE", "SketchBSpline_164"),
                  model.selection("EDGE", "SketchBSpline_165"),
                  model.selection("EDGE", "SketchBSpline_166"),
                  model.selection("EDGE", "SketchBSpline_167"),
                  model.selection("EDGE", "SketchBSpline_168"),
                  model.selection("EDGE", "SketchBSpline_169"),
                  model.selection("EDGE", "SketchBSpline_170"),
                  model.selection("EDGE", "SketchBSpline_171"),
                  model.selection("EDGE", "SketchBSpline_172"),
                  model.selection("EDGE", "SketchBSpline_173"),
                  model.selection("EDGE", "SketchBSpline_174"),
                  model.selection("EDGE", "SketchBSpline_175"),
                  model.selection("EDGE", "SketchBSpline_176"),
                  model.selection("EDGE", "SketchBSpline_177"),
                  model.selection("EDGE", "SketchBSpline_178"),
                  model.selection("EDGE", "SketchBSpline_179"),
                  model.selection("EDGE", "SketchBSpline_180"),
                  model.selection("EDGE", "SketchBSpline_181"),
                  model.selection("EDGE", "SketchBSpline_182"),
                  model.selection("EDGE", "SketchArc_3_2")]
Wire_5 = model.addWire(Part_1_doc, Wire_5_objects, False)

### Create Wire
Wire_6_objects = [model.selection("EDGE", "SketchBSpline_183"),
                  model.selection("EDGE", "SketchBSpline_184"),
                  model.selection("EDGE", "SketchBSpline_185"),
                  model.selection("EDGE", "SketchBSpline_186"),
                  model.selection("EDGE", "SketchBSpline_187"),
                  model.selection("EDGE", "SketchBSpline_188"),
                  model.selection("EDGE", "SketchBSpline_189"),
                  model.selection("EDGE", "SketchBSpline_190"),
                  model.selection("EDGE", "SketchBSpline_191"),
                  model.selection("EDGE", "SketchBSpline_192"),
                  model.selection("EDGE", "SketchBSpline_193"),
                  model.selection("EDGE", "SketchBSpline_194"),
                  model.selection("EDGE", "SketchBSpline_195"),
                  model.selection("EDGE", "SketchBSpline_196"),
                  model.selection("EDGE", "SketchBSpline_197"),
                  model.selection("EDGE", "SketchBSpline_198"),
                  model.selection("EDGE", "SketchBSpline_199"),
                  model.selection("EDGE", "SketchBSpline_200"),
                  model.selection("EDGE", "SketchBSpline_201"),
                  model.selection("EDGE", "SketchBSpline_202"),
                  model.selection("EDGE", "SketchBSpline_203"),
                  model.selection("EDGE", "SketchBSpline_204"),
                  model.selection("EDGE", "SketchBSpline_205"),
                  model.selection("EDGE", "SketchBSpline_206"),
                  model.selection("EDGE", "SketchBSpline_207"),
                  model.selection("EDGE", "SketchBSpline_208"),
                  model.selection("EDGE", "SketchBSpline_209"),
                  model.selection("EDGE", "SketchBSpline_210"),
                  model.selection("EDGE", "SketchBSpline_211"),
                  model.selection("EDGE", "SketchBSpline_212"),
                  model.selection("EDGE", "SketchBSpline_213"),
                  model.selection("EDGE", "SketchBSpline_214"),
                  model.selection("EDGE", "SketchBSpline_215"),
                  model.selection("EDGE", "SketchBSpline_216"),
                  model.selection("EDGE", "SketchBSpline_217"),
                  model.selection("EDGE", "SketchBSpline_218"),
                  model.selection("EDGE", "SketchBSpline_219"),
                  model.selection("EDGE", "SketchBSpline_220"),
                  model.selection("EDGE", "SketchBSpline_221"),
                  model.selection("EDGE", "SketchBSpline_222"),
                  model.selection("EDGE", "SketchBSpline_223"),
                  model.selection("EDGE", "SketchBSpline_224"),
                  model.selection("EDGE", "SketchBSpline_225"),
                  model.selection("EDGE", "SketchBSpline_226"),
                  model.selection("EDGE", "SketchBSpline_227"),
                  model.selection("EDGE", "SketchBSpline_228"),
                  model.selection("EDGE", "SketchBSpline_229"),
                  model.selection("EDGE", "SketchBSpline_230"),
                  model.selection("EDGE", "SketchBSpline_231"),
                  model.selection("EDGE", "SketchBSpline_232"),
                  model.selection("EDGE", "SketchBSpline_233"),
                  model.selection("EDGE", "SketchBSpline_234"),
                  model.selection("EDGE", "SketchBSpline_235"),
                  model.selection("EDGE", "SketchBSpline_236"),
                  model.selection("EDGE", "SketchBSpline_237"),
                  model.selection("EDGE", "SketchBSpline_238"),
                  model.selection("EDGE", "SketchBSpline_239"),
                  model.selection("EDGE", "SketchBSpline_240"),
                  model.selection("EDGE", "SketchBSpline_241"),
                  model.selection("EDGE", "SketchBSpline_242"),
                  model.selection("EDGE", "SketchArc_4_2")]
Wire_6 = model.addWire(Part_1_doc, Wire_6_objects, False)

### Create Filling
Filling_1 = model.addFilling(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_4_1")], "curve_info", 2, 12, 0, 1e-06, 1e-06, False)

### Create Filling
Filling_2 = model.addFilling(Part_1_doc, [model.selection("WIRE", "Wire_2_1"), model.selection("WIRE", "Wire_5_1")], "curve_info", 2, 12, 0, 1e-06, 1e-06, False)

### Create Filling
Filling_3 = model.addFilling(Part_1_doc, [model.selection("WIRE", "Wire_3_1"), model.selection("WIRE", "Wire_6_1")], "curve_info", 2, 5, 0, 0.001, 0.001, False)

model.end()

from GeomAPI import *

model.testNbResults(Filling_1, 1)
model.testNbSubResults(Filling_1, [0])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.VERTEX, [8])
model.testResultsAreas(Filling_1, [28.94082894])

model.testNbResults(Filling_2, 1)
model.testNbSubResults(Filling_2, [0])
model.testNbSubShapes(Filling_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Filling_2, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Filling_2, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Filling_2, GeomAPI_Shape.VERTEX, [8])
model.testResultsAreas(Filling_2, [29.10364506])

model.testNbResults(Filling_3, 1)
model.testNbSubResults(Filling_3, [0])
model.testNbSubShapes(Filling_3, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Filling_3, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Filling_3, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Filling_3, GeomAPI_Shape.VERTEX, [8])
model.testResultsAreas(Filling_3, [30.744277238])

assert(model.checkPythonDump())
