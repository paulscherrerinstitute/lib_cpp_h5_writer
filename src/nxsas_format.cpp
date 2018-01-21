#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "H5Format.hpp"

using namespace std;

h5_group* get_format_definition(){

    auto format = 
    new h5_group("entry", {
        new h5_attr("NX_class", "NXentry", NX_CHAR),

        new h5_dataset("start_time", "entry/start_time", NX_DATE_TIME, {
            new h5_attr("NX_class", "NX_DATE_TIME", NX_CHAR)
        }),
        new h5_dataset("end_time", "entry/end_time", NX_DATE_TIME, {
            new h5_attr("NX_class", "NX_DATE_TIME", NX_CHAR)
        }),
        new h5_dataset("definition", "entry/definition", NX_CHAR, {
            new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
        }),
        new h5_dataset("title", "entry/title", NX_CHAR, {
            new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
        }),

        new h5_group("control", {
            new h5_attr("NX_class", "NXmonitor", NX_CHAR),

            new h5_dataset("mode", "control/mode", NX_CHAR, {
                new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
            }),
            new h5_dataset("note", "control/note", NXnote, {
                new h5_attr("NX_class", "NXnote", NX_CHAR)
            }),
            new h5_dataset("integral", "control/integral", NX_FLOAT, {
                new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
            })
        }),

        new h5_group("instrument", {
            new h5_attr("NX_class", "NXinstrument", NX_CHAR),

            new h5_dataset("name", "instrument/name", NX_CHAR, {
                new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
            }),

            new h5_group("insertion_device", {
                new h5_attr("NX_class", "NXinsertion_device", NX_CHAR),

                new h5_dataset("gap", "insertion_device/gap", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("k", "insertion_device/k", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_DIMENSIONLESS", "True", NX_CHAR)
                }),
                new h5_dataset("harmonic", "insertion_device/harmonic", NX_INT, {
                    new h5_attr("NX_UNITLESS", "true", NX_CHAR),
                    new h5_attr("NX_class", "NX_INT", NX_CHAR)
                }),
                new h5_dataset("length", "insertion_device/length", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("type", "insertion_device/type", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                })
            }),

            new h5_group("XBPM5", {
                new h5_attr("NX_class", "NXdetector_group", NX_CHAR),

                new h5_dataset("group_parent", "XBPM5/group_parent", NX_INT, {
                    new h5_attr("NX_class", "NX_INT", NX_CHAR)
                }),
                new h5_dataset("group_names", "XBPM5/group_names", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("group_index", "XBPM5/group_index", NX_INT, {
                    new h5_attr("NX_class", "NX_INT", NX_CHAR)
                }),

                new h5_group("XBPM5_x", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("data", "XBPM5_x/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM5_x/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    })
                }),

                new h5_group("XBPM5_y", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("description", "XBPM5_y/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("data", "XBPM5_y/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                }),

                new h5_group("XBPM5_sum", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("data", "XBPM5_sum/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM5_sum/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("saturation_value", "XBPM5_sum/saturation_value", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                }),

                new h5_group("XBPM5_skew", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("description", "XBPM5_skew/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("data", "XBPM5_skew/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                }),

                new h5_group("XBPM5", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("description", "XBPM5/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("distance", "XBPM5/distance", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("count_time", "XBPM5/count_time", NX_NUMBER, {
                        new h5_attr("NX_class", "NX_NUMBER", NX_CHAR),
                        new h5_attr("NX_TIME", "s", NX_CHAR)
                    }),
                    new h5_dataset("calibration_date", "XBPM5/calibration_date", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("type", "XBPM5/type", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("gain_setting", "XBPM5/gain_setting", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                })
            }),

            new h5_group("XBPM4", {
                new h5_attr("NX_class", "NXdetector_group", NX_CHAR),

                new h5_dataset("group_names", "XBPM4/group_names", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("group_index", "XBPM4/group_index", NX_INT, {
                    new h5_attr("NX_class", "NX_INT", NX_CHAR)
                }),
                new h5_dataset("group_parent", "XBPM4/group_parent", NX_INT, {
                    new h5_attr("NX_class", "NX_INT", NX_CHAR)
                }),

                new h5_group("XBPM4_sum", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("data", "XBPM4_sum/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("saturation_value", "XBPM4_sum/saturation_value", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM4_sum/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    })
                }),

                new h5_group("XBPM4_skew", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("data", "XBPM4_skew/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM4_skew/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    })
                }),

                new h5_group("XBPM4_x", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("description", "XBPM4_x/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("data", "XBPM4_x/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                }),

                new h5_group("XBPM4", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("distance", "XBPM4/distance", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("calibration_date", "XBPM4/calibration_date", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM4/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("type", "XBPM4/type", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("gain_setting", "XBPM4/gain_setting", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("count_time", "XBPM4/count_time", NX_NUMBER, {
                        new h5_attr("NX_TIME", "s", NX_CHAR),
                        new h5_attr("NX_class", "NX_NUMBER", NX_CHAR)
                    })
                }),

                new h5_group("XBPM4_y", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("data", "XBPM4_y/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM4_y/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    })
                })
            }),

            new h5_group("beam_stop_2", {
                new h5_attr("NX_class", "NX_beamstop", NX_CHAR),

                new h5_dataset("y", "beam_stop_2/y", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("status", "beam_stop_2/status", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("size_x", "beam_stop_2/size_x", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("data", "beam_stop_2/data", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("description", "beam_stop_2/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("size_y", "beam_stop_2/size_y", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x", "beam_stop_2/x", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("distance_to_detector", "beam_stop_2/distance_to_detector", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                })
            }),

            new h5_group("beam_stop_1", {
                new h5_attr("NX_class", "NX_beamstop", NX_CHAR),

                new h5_dataset("size", "beam_stop_1/size", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("y", "beam_stop_1/y", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("description", "beam_stop_1/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("distance_to_detector", "beam_stop_1/distance_to_detector", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x", "beam_stop_1/x", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("status", "beam_stop_1/status", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                })
            }),

            new h5_group("filter_set", {
                new h5_attr("NX_class", "NXattenuator", NX_CHAR),

                new h5_dataset("type", "filter_set/type", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("distance", "filter_set/distance", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("description", "filter_set/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("attenuator_transmission", "filter_set/attenuator_transmission", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                })
            }),

            new h5_group("slit_4", {
                new h5_attr("NX_class", "NXslit", NX_CHAR),

                new h5_dataset("height", "slit_4/height", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("distance", "slit_4/distance", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x_gap", "slit_4/x_gap", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("description", "slit_4/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("y_gap", "slit_4/y_gap", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("material", "slit_4/material", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("x_translation", "slit_4/x_translation", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                })
            }),

            new h5_group("slit_2", {
                new h5_attr("NX_class", "NXslit", NX_CHAR),

                new h5_dataset("y_gap", "slit_2/y_gap", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("distance", "slit_2/distance", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x_gap", "slit_2/x_gap", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x_translation", "slit_2/x_translation", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("description", "slit_2/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("height", "slit_2/height", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("material", "slit_2/material", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                })
            }),

            new h5_group("source", {
                new h5_attr("NX_class", "NXsource", NX_CHAR),

                new h5_dataset("divergence_y", "source/divergence_y", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_ANGLE", "radians", NX_CHAR)
                }),
                new h5_dataset("divergence_x", "source/divergence_x", NX_FLOAT, {
                    new h5_attr("NX_ANGLE", "radians", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("current", "source/current", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_CURRENT", "mA", NX_CHAR)
                }),
                new h5_dataset("distance", "source/distance", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("sigma_x", "source/sigma_x", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("type", "source/type", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("probe", "source/probe", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("sigma_y", "source/sigma_y", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("name", "source/name", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                })
            }),

            new h5_group("mirror", {
                new h5_attr("NX_class", "NXmirror", NX_CHAR),

                new h5_dataset("description", "mirror/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("substrate_material", "mirror/substrate_material", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("distance", "mirror/distance", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("bend_y", "mirror/bend_y", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_ANY", "", NX_CHAR)
                }),
                new h5_dataset("incident_angle", "mirror/incident_angle", NX_FLOAT, {
                    new h5_attr("NX_ANGLE", "degrees", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("coating_material", "mirror/coating_material", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("type", "mirror/type", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                })
            }),

            new h5_group("slit_3", {
                new h5_attr("NX_class", "NXslit", NX_CHAR),

                new h5_dataset("y_gap", "slit_3/y_gap", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("distance", "slit_3/distance", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("height", "slit_3/height", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x_translation", "slit_3/x_translation", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("material", "slit_3/material", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("description", "slit_3/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("x_gap", "slit_3/x_gap", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                })
            }),

            new h5_group("slit_1", {
                new h5_attr("NX_class", "NXslit", NX_CHAR),

                new h5_dataset("x_gap", "slit_1/x_gap", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x_translation", "slit_1/x_translation", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("y_gap", "slit_1/y_gap", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("description", "slit_1/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("material", "slit_1/material", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("distance", "slit_1/distance", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("height", "slit_1/height", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                })
            }),

            new h5_group("monochromator", {
                new h5_attr("NX_class", "NXmonochromator", NX_CHAR),

                new h5_dataset("wavelength", "monochromator/wavelength", NX_FLOAT, {
                    new h5_attr("NX_WAVELENGTH", "Angstrom", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("wavelength_spread", "monochromator/wavelength_spread", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("distance", "monochromator/distance", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("energy", "monochromator/energy", NX_FLOAT, {
                    new h5_attr("NX_ENERGY", "keV", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("type", "monochromator/type", NXnote, {
                    new h5_attr("NX_class", "NXnote", NX_CHAR)
                }),

                new h5_group("crystal_2", {
                    new h5_attr("NX_class", "NXcrystal", NX_CHAR),

                    new h5_dataset("bragg_angle", "crystal_2/bragg_angle", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                        new h5_attr("NX_ANGLE", "degrees", NX_CHAR)
                    }),
                    new h5_dataset("type", "crystal_2/type", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("temperature", "crystal_2/temperature", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("reflection", "crystal_2/reflection", NX_INT, {
                        new h5_attr("NX_class", "NX_INT", NX_CHAR),
                        new h5_attr("NX_UNITLESS", "", NX_CHAR)
                    }),
                    new h5_dataset("order_no", "crystal_2/order_no", NX_INT, {
                        new h5_attr("NX_class", "NX_INT", NX_CHAR)
                    }),
                    new h5_dataset("usage", "crystal_2/usage", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("bend_x", "crystal_2/bend_x", NX_FLOAT, {
                        new h5_attr("NX_ANY", "", NX_CHAR),
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                }),

                new h5_group("crystal_1", {
                    new h5_attr("NX_class", "NXcrystal", NX_CHAR),

                    new h5_dataset("type", "crystal_1/type", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("temperature", "crystal_1/temperature", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("usage", "crystal_1/usage", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("bragg_angle", "crystal_1/bragg_angle", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                        new h5_attr("NX_ANGLE", "degrees", NX_CHAR)
                    }),
                    new h5_dataset("order_no", "crystal_1/order_no", NX_INT, {
                        new h5_attr("NX_class", "NX_INT", NX_CHAR)
                    }),
                    new h5_dataset("reflection", "crystal_1/reflection", NX_INT, {
                        new h5_attr("NX_UNITLESS", "", NX_CHAR),
                        new h5_attr("NX_class", "NX_INT", NX_CHAR)
                    })
                })
            }),

            new h5_group("XBPM6", {
                new h5_attr("NX_class", "NXdetector_group", NX_CHAR),

                new h5_dataset("group_index", "XBPM6/group_index", NX_INT, {
                    new h5_attr("NX_class", "NX_INT", NX_CHAR)
                }),
                new h5_dataset("group_names", "XBPM6/group_names", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("group_parent", "XBPM6/group_parent", NX_INT, {
                    new h5_attr("NX_class", "NX_INT", NX_CHAR)
                }),

                new h5_group("XBPM6_skew", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("data", "XBPM6_skew/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM6_skew/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    })
                }),

                new h5_group("XBPM6", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("distance", "XBPM6/distance", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("gain_setting", "XBPM6/gain_setting", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("calibration_date", "XBPM6/calibration_date", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM6/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("count_time", "XBPM6/count_time", NX_NUMBER, {
                        new h5_attr("NX_TIME", "s", NX_CHAR),
                        new h5_attr("NX_class", "NX_NUMBER", NX_CHAR)
                    }),
                    new h5_dataset("type", "XBPM6/type", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    })
                }),

                new h5_group("XBPM6_sum", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("description", "XBPM6_sum/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("data", "XBPM6_sum/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("saturation_value", "XBPM6_sum/saturation_value", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                }),

                new h5_group("XBPM6_y", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("data", "XBPM6_y/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    }),
                    new h5_dataset("description", "XBPM6_y/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    })
                }),

                new h5_group("XBPM6_x", {
                    new h5_attr("NX_class", "NX_detector", NX_CHAR),

                    new h5_dataset("description", "XBPM6_x/description", NX_CHAR, {
                        new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                    }),
                    new h5_dataset("data", "XBPM6_x/data", NX_FLOAT, {
                        new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                    })
                })
            }),

            new h5_group("slit_0", {
                new h5_attr("NX_class", "NXslit", NX_CHAR),

                new h5_dataset("distance", "slit_0/distance", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("material", "slit_0/material", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                }),
                new h5_dataset("x_translation", "slit_0/x_translation", NX_FLOAT, {
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR),
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR)
                }),
                new h5_dataset("x_gap", "slit_0/x_gap", NX_FLOAT, {
                    new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                    new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
                }),
                new h5_dataset("description", "slit_0/description", NX_CHAR, {
                    new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
                })
            })
        }),

        new h5_group("plottable_data", {
            new h5_attr("NX_class", "NXdata", NX_CHAR)
        }),

        new h5_group("sample", {
            new h5_attr("NX_class", "NXsample", NX_CHAR),

            new h5_dataset("x_translation", "sample/x_translation", NX_FLOAT, {
                new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
            }),
            new h5_dataset("temperature_log", "sample/temperature_log", NX_FLOAT, {
                new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
            }),
            new h5_dataset("description", "sample/description", NX_CHAR, {
                new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
            }),
            new h5_dataset("name", "sample/name", NX_CHAR, {
                new h5_attr("NX_class", "NX_CHAR", NX_CHAR)
            }),
            new h5_dataset("y_translation", "sample/y_translation", NX_FLOAT, {
                new h5_attr("NX_LENGTH", "mm", NX_CHAR),
                new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
            }),
            new h5_dataset("aequatorial_angle", "sample/aequatorial_angle", NX_FLOAT, {
                new h5_attr("NX_ANGLE", "deg", NX_CHAR),
                new h5_attr("NX_class", "NX_FLOAT", NX_CHAR)
            })
        })
    });

    return format;
}

map<string, boost::any>* get_default_values(){
    auto default_values = new std::map<std::basic_string<char>, boost::any>(
        {    
            {"filter_set/description", "The filter set consists of 4 linear stages, each with five filter positions. Additionally each one allows for an out position to allow no filtering."},
            {"XBPM4/calibration_date", "???"},
            {"crystal_2/type", "Si"},
            {"beam_stop_2/size_y", 2.25},
            {"source/name", "Swiss Light Source"},
            {"beam_stop_2/size_x", 5.0},
            {"slit_1/material", "OFHC Cu"},
            {"monochromator/type", "Double crystal fixed exit monochromator"},
            {"XBPM4_skew/description", "Normalized difference of counts between diagonal quadrants"},
            {"XBPM6/type", "???"},
            {"XBPM6/description", "Four quadrant x-ray beam position monitor"},
            {"slit_3/material", "Si"},
            {"XBPM5/group_index", 3},
            {"crystal_1/type", "Si"},
            {"insertion_device/length", 1820.0},
            {"XBPM5_skew/description", "Normalized difference of counts between diagonal quadrants"},
            {"XBPM5_y/description", "Normalized difference of counts between high and low quadrants"},
            {"source/sigma_x", 0.202},
            {"XBPM5/type", "???"},
            {"XBPM4_x/description", "Normalized difference of counts between left and right quadrants"},
            {"XBPM4/group_names", "XBPM4, XBPM4_sum, XBPM4_x, XBPM4_y, XBPM4_skew"},
            {"slit_2/description", "Slit 2, optics hutch"},
            {"mirror/substrate_material", "SiO2"},
            {"XBPM5/group_names", "XBPM5, XBPM5_sum, XBPM5_x, XBPM5_y, XBPM5_skew"},
            {"control/note", "monitor"},
            {"monochromator/wavelength_spread", 0.0001},
            {"XBPM6/calibration_date", "???"},
            {"source/divergence_x", 0.000135},
            {"XBPM4_y/description", "Normalized difference of counts between high and low quadrants"},
            {"XBPM6_sum/description", "Sum of counts for the four quadrants"},
            {"crystal_1/order_no", 1},
            {"slit_3/description", "Slit 3, experimental hutch, exposure box"},
            {"XBPM5_sum/description", "Sum of counts for the four quadrants"},
            {"insertion_device/k", 2.46},
            {"insertion_device/type", "undulator"},
            {"XBPM6/group_index", 4},
            {"entry/definition", "NXsas"},
            {"crystal_2/usage", "Bragg"},
            {"entry/end_time", "???"},
            {"XBPM6_skew/description", "Normalized difference of counts between diagonal quadrants"},
            {"XBPM6_y/description", "Normalized difference of counts between high and low quadrants"},
            {"crystal_2/order_no", 2},
            {"XBPM6_x/description", "Normalized difference of counts between left and right quadrants"},
            {"mirror/type", "single"},
            {"sample/aequatorial_angle", 1.0},
            {"XBPM6/group_names", "XBPM6, XBPM6_sum, XBPM6_x, XBPM6_y, XBPM6_skew"},
            {"slit_4/material", "Si"},
            {"beam_stop_1/description", "circular"},
            {"source/sigma_y", 0.018},
            {"XBPM5/description", "Four quadrant x-ray beam position monitor"},
            {"slit_2/material", "Ag"},
            {"XBPM4/distance", 0.0},
            {"beam_stop_1/size", 3.0},
            {"XBPM5_x/description", "Normalized difference of counts between left and right quadrants"},
            {"XBPM5/calibration_date", "???"},
            {"XBPM4/type", "???"},
            {"XBPM4_sum/description", "Sum of counts for the four quadrants"},
            {"slit_0/description", "Horizontal secondary source slit"},
            {"source/divergence_y", 2.5e-05},
            {"crystal_1/usage", "Bragg"},
            {"slit_4/description", "Slit 4, experimental hutch, exposure box"},
            {"control/mode", "Value from bpm4s"},
            {"XBPM4/description", "Four quadrant x-ray beam position monitor"},
            {"XBPM5/distance", 0.0},
            {"XBPM6/distance", 0.0},
            {"source/probe", "x-ray"},
            {"slit_1/description", "Slit 1, optics hutch"},
            {"instrument/name", "cSAXS beamline"},
            {"XBPM4/group_index", 2},
            {"mirror/description", "Grazing incidence mirror to reject high-harmonic wavelengths from the monochromator. There are three coating options available that are used depending on the X-ray energy, no coating (SiO2), rhodium (Rh) or platinum (Pt)."},
            {"slit_0/material", "OFHC Cu"},
            {"beam_stop_2/description", "rectangular"},
            {"source/type", "Synchrotron X-ray Source"},
            // TODO: This are temp for dataset arrays. Implement size based on type.
            {"crystal_1/reflection", 1},
            {"crystal_2/reflection", 1},
            {"XBPM4/group_parent", 1},
            {"XBPM5/group_parent", 1},
            {"XBPM6/group_parent", 1},
        }
    );

    return default_values;
}

void add_calculated_values(map<string, boost::any>& values){

    map<string, string> input_mapping = {
        {"source/distance", "input/samz"},
        {"slit_0/distance", "input/samz"},
        {"slit_1/distance", "input/samz"},
        {"monochromator/distance", "input/samz"},
        {"mirror/distance", "input/samz"},
        {"slit_2/distance", "input/samz"},
        {"slit_3/distance", "input/samz"},
        {"filter_set/distance", "input/samz"},
        {"slit_4/distance", "input/samz"},
        {"monochromator/wavelength", "monochromator/energy"},
        {"filter_set/attenuator_transmission", "input/ftrans"},
    };
    
    map<string, function<double(double)>> functions = {
        {"source/distance", [](double x) -> double { return -33800 - x;}},
        {"slit_0/distance", [](double x) -> double { return -33800 + 12100 - x;}},
        {"slit_1/distance", [](double x) -> double { return -33800 + 26000 - x;}},
        {"monochromator/distance", [](double x) -> double { return -33800 - x;}},
        {"mirror/distance", [](double x) -> double { return -33800 + 29430 - x;}},
        {"slit_2/distance", [](double x) -> double { return -33800 + 30660 - x;}},
        {"slit_3/distance", [](double x) -> double { return 0 - x;}},
        {"filter_set/distance", [](double x) -> double { return 0 - x;}},
        {"slit_4/distance", [](double x) -> double { return 0 - x;}},
        {"monochromator/wavelength", [](double x) -> double { return 12.3984193 / x;}},
        {"filter_set/attenuator_transmission", [](double x) -> double { return pow(10.0, x);}},
    };

    for (auto pair : functions) {
        string input_name;

        try{
            input_name = input_mapping.at(pair.first);
        } catch (const out_of_range& exception){
            stringstream error_message;
            error_message << "Cannot find input_name for calculated value " << pair.first << "." << endl;

            throw runtime_error(error_message.str());
        }
        
        try {
            auto input_value = values.at(input_name);
            values[pair.first] = pair.second(boost::any_cast<double>(input_value));

        } catch (const boost::bad_any_cast& exception) {
            stringstream error_message;
            error_message << "Cannot convert input_name " << input_name << " to double for calculated value " << pair.first << "." << endl;

            throw runtime_error(error_message.str());

        } catch (const out_of_range& exception){
            stringstream error_message;
            error_message << "No input value " << input_name << " found in values for calculated value " << pair.first << "." << endl;

            throw runtime_error(error_message.str());
        }
    }
}

void add_input_values(map<string, boost::any>& values, map<string, boost::any>& input_values) {
    map<string, list<string>> input_mapping = {
        {"sl2wv", {"slit_2/y_gap"}},
        {"sl0ch", {"slit_0/x_translation"}},
        {"sl2wh", {"slit_2/x_gap"}},
        {"temp_mono_cryst_1", {"crystal_1/temperature"}},
        {"harmonic", {"insertion_device/harmonic"}},
        {"mokev", {"monochromator/energy"}},
        {"sl2cv", {"slit_2/height"}},
        {"bpm4_gain_setting", {"XBPM4/gain_setting"}},
        {"mirror_coating", {"mirror/coating_material"}},
        {"samx", {"sample/x_translation"}},
        {"sample_name", {"sample/name"}},
        {"bpm5y", {"XBPM5_y/data"}},
        {"sl2ch", {"slit_2/x_translation"}},
        {"curr", {"source/current"}},
        {"bs2_status", {"beam_stop_2/status"}},
        {"bs2y", {"beam_stop_2/y"}},
        {"diode", {"beam_stop_2/data"}},
        {"samy", {"sample/y_translation"}},
        {"sl4ch", {"slit_4/x_translation"}},
        {"sl4wh", {"slit_4/x_gap"}},
        {"temp_mono_cryst_2", {"crystal_2/temperature"}},
        {"sl3wh", {"slit_3/x_gap"}},
        {"mith", {"mirror/incident_angle"}},
        {"bs1_status", {"beam_stop_1/status"}},
        {"bpm4s", {"XBPM4_sum/data", "control/integral"}},
        {"sl0wh", {"slit_0/x_gap"}},
        {"bpm6z", {"XBPM6_skew/data"}},
        {"bs1y", {"beam_stop_1/y"}},
        {"scan", {"entry/title"}},
        {"bpm5_gain_setting", {"XBPM5/gain_setting"}},
        {"bpm4z", {"XBPM4_skew/data"}},
        {"bpm4x", {"XBPM4_x/data"}},
        {"date", {"entry/start_time"}},
        {"mibd", {"mirror/bend_y"}},
        {"temp", {"sample/temperature_log"}},
        {"idgap", {"insertion_device/gap"}},
        {"sl4cv", {"slit_4/height"}},
        {"sl1wv", {"slit_1/y_gap"}},
        {"sl3wv", {"slit_3/y_gap"}},
        {"sl1ch", {"slit_1/x_translation"}},
        {"bs2x", {"beam_stop_2/x"}},
        {"bpm6_gain_setting", {"XBPM6/gain_setting"}},
        {"bpm4y", {"XBPM4_y/data"}},
        {"bpm6s", {"XBPM6_sum/data"}},
        {"sample_description", {"sample/description"}},
        {"bpm5z", {"XBPM5_skew/data"}},
        {"moth1", {"crystal_1/bragg_angle", "crystal_2/bragg_angle"}},
        {"sec", {"XBPM4/count_time", "XBPM5/count_time", "XBPM6/count_time"}},
        {"sl3cv", {"slit_3/height"}},
        {"bs1x", {"beam_stop_1/x"}},
        {"bpm6_saturation_value", {"XBPM6_sum/saturation_value"}},
        {"bpm5s", {"XBPM5_sum/data"}},
        {"mobd", {"crystal_2/bend_x"}},
        {"sl1wh", {"slit_1/x_gap"}},
        {"sl4wv", {"slit_4/y_gap"}},
        {"bs2_det_dist", {"beam_stop_2/distance_to_detector"}},
        {"bpm5_saturation_value", {"XBPM5_sum/saturation_value"}},
        {"fil_comb_description", {"filter_set/type"}},
        {"bpm5x", {"XBPM5_x/data"}},
        {"bpm4_saturation_value", {"XBPM4_sum/saturation_value"}},
        {"bs1_det_dist", {"beam_stop_1/distance_to_detector"}},
        {"sl3ch", {"slit_3/x_translation"}},
        {"bpm6y", {"XBPM6_y/data"}},
        {"sl1cv", {"slit_1/height"}},
        {"bpm6x", {"XBPM6_x/data"}},
        {"ftrans", {"input/ftrans"}},
        {"samz", {"input/samz"}},
    };

    for (auto input : input_mapping) {
        for (auto destination_name : input.second) {
            values[destination_name] = input_values[input.first];
        }
    }
}

std::map<string, DATA_TYPE>* get_input_value_type() {
    auto input_value_types = new std::map<string, DATA_TYPE>({
        {"sl2wv", NX_FLOAT},
        {"sl0ch", NX_FLOAT},
        {"sl2wh", NX_FLOAT},
        {"temp_mono_cryst_1", NX_FLOAT},
        {"harmonic", NX_INT},
        {"mokev", NX_FLOAT},
        {"sl2cv", NX_FLOAT},
        {"bpm4_gain_setting", NX_FLOAT},
        {"mirror_coating", NX_CHAR},
        {"samx", NX_FLOAT},
        {"sample_name", NX_CHAR},
        {"bpm5y", NX_FLOAT},
        {"sl2ch", NX_FLOAT},
        {"curr", NX_FLOAT},
        {"bs2_status", NX_CHAR},
        {"bs2y", NX_FLOAT},
        {"diode", NX_FLOAT},
        {"samy", NX_FLOAT},
        {"sl4ch", NX_FLOAT},
        {"sl4wh", NX_FLOAT},
        {"temp_mono_cryst_2", NX_FLOAT},
        {"sl3wh", NX_FLOAT},
        {"mith", NX_FLOAT},
        {"bs1_status", NX_CHAR},
        {"bpm4s", NX_FLOAT},
        {"sl0wh", NX_FLOAT},
        {"bpm6z", NX_FLOAT},
        {"bs1y", NX_FLOAT},
        {"scan", NX_CHAR},
        {"bpm5_gain_setting", NX_FLOAT},
        {"bpm4z", NX_FLOAT},
        {"bpm4x", NX_FLOAT},
        {"date", NX_DATE_TIME},
        {"mibd", NX_FLOAT},
        {"temp", NX_FLOAT},
        {"idgap", NX_FLOAT},
        {"sl4cv", NX_FLOAT},
        {"sl1wv", NX_FLOAT},
        {"sl3wv", NX_FLOAT},
        {"sl1ch", NX_FLOAT},
        {"bs2x", NX_FLOAT},
        {"bpm6_gain_setting", NX_FLOAT},
        {"bpm4y", NX_FLOAT},
        {"bpm6s", NX_FLOAT},
        {"sample_description", NX_CHAR},
        {"bpm5z", NX_FLOAT},
        {"moth1", NX_FLOAT},
        {"sec", NX_NUMBER},
        {"sl3cv", NX_FLOAT},
        {"bs1x", NX_FLOAT},
        {"bpm6_saturation_value", NX_FLOAT},
        {"bpm5s", NX_FLOAT},
        {"mobd", NX_FLOAT},
        {"sl1wh", NX_FLOAT},
        {"sl4wv", NX_FLOAT},
        {"bs2_det_dist", NX_FLOAT},
        {"bpm5_saturation_value", NX_FLOAT},
        {"fil_comb_description", NX_CHAR},
        {"bpm5x", NX_FLOAT},
        {"bpm4_saturation_value", NX_FLOAT},
        {"bs1_det_dist", NX_FLOAT},
        {"sl3ch", NX_FLOAT},
        {"bpm6y", NX_FLOAT},
        {"sl1cv", NX_FLOAT},
        {"bpm6x", NX_FLOAT},
        {"ftrans", NX_FLOAT},
        {"samz", NX_FLOAT},
    });

    return input_value_types;
}