#include "h5_file_format.hpp"


h5_group get_format(){

    auto format = 
    h5_group("entry", {
        h5_attribute_val("NX_class", "NXentry"),

        h5_dataset("title", "entry/title", NX_CHAR, {
            h5_attribute_val("NX_class", "NX_CHAR")
        }),
        h5_dataset("definition", "entry/definition", NX_CHAR, {
            h5_attribute_val("NX_class", "NX_CHAR")
        }),
        h5_dataset("end_time", "entry/end_time", NX_DATE_TIME, {
            h5_attribute_val("NX_class", "NX_DATE_TIME")
        }),
        h5_dataset("start_time", "entry/start_time", NX_DATE_TIME, {
            h5_attribute_val("NX_class", "NX_DATE_TIME")
        }),

        h5_group("control", {
            h5_attribute_val("NX_class", "NXmonitor"),

            h5_dataset("integral", "control/integral", NX_FLOAT, {
                h5_attribute_val("NX_class", "NX_FLOAT")
            }),
            h5_dataset("mode", "control/mode", NX_CHAR, {
                h5_attribute_val("NX_class", "NX_CHAR")
            }),
            h5_dataset("note", "control/note", NXnote, {
                h5_attribute_val("NX_class", "NXnote")
            })
        }),

        h5_group("plottable_data", {
            h5_attribute_val("NX_class", "NXdata")
        }),

        h5_group("sample", {
            h5_attribute_val("NX_class", "NXsample"),

            h5_dataset("aequatorial_angle", "sample/aequatorial_angle", NX_FLOAT, {
                h5_attribute_val("NX_class", "NX_FLOAT"),
                h5_attribute_val("NX_ANGLE", "deg")
            }),
            h5_dataset("description", "sample/description", NX_CHAR, {
                h5_attribute_val("NX_class", "NX_CHAR")
            }),
            h5_dataset("name", "sample/name", NX_CHAR, {
                h5_attribute_val("NX_class", "NX_CHAR")
            }),
            h5_dataset("y_translation", "sample/y_translation", NX_FLOAT, {
                h5_attribute_val("NX_class", "NX_FLOAT"),
                h5_attribute_val("NX_LENGTH", "mm")
            }),
            h5_dataset("x_translation", "sample/x_translation", NX_FLOAT, {
                h5_attribute_val("NX_LENGTH", "mm"),
                h5_attribute_val("NX_class", "NX_FLOAT")
            }),
            h5_dataset("temperature_log", "sample/temperature_log", NX_FLOAT, {
                h5_attribute_val("NX_class", "NX_FLOAT")
            })
        }),

        h5_group("instrument", {
            h5_attribute_val("NX_class", "NXinstrument"),

            h5_dataset("name", "instrument/name", NX_CHAR, {
                h5_attribute_val("NX_class", "NX_CHAR")
            }),

            h5_group("slit_3", {
                h5_attribute_val("NX_class", "NXslit"),

                h5_dataset("y_gap", "slit_3/y_gap", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("height", "slit_3/height", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("description", "slit_3/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("x_gap", "slit_3/x_gap", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("material", "slit_3/material", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("x_translation", "slit_3/x_translation", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("distance", "slit_3/distance", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            }),

            h5_group("beam_stop_2", {
                h5_attribute_val("NX_class", "NX_beamstop"),

                h5_dataset("status", "beam_stop_2/status", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("y", "beam_stop_2/y", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("description", "beam_stop_2/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("size_y", "beam_stop_2/size_y", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("data", "beam_stop_2/data", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("distance_to_detector", "beam_stop_2/distance_to_detector", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("x", "beam_stop_2/x", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("size_x", "beam_stop_2/size_x", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            }),

            h5_group("slit_2", {
                h5_attribute_val("NX_class", "NXslit"),

                h5_dataset("x_translation", "slit_2/x_translation", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("y_gap", "slit_2/y_gap", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("x_gap", "slit_2/x_gap", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("height", "slit_2/height", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("description", "slit_2/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("material", "slit_2/material", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("distance", "slit_2/distance", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            }),

            h5_group("mirror", {
                h5_attribute_val("NX_class", "NXmirror"),

                h5_dataset("bend_y", "mirror/bend_y", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_ANY", "")
                }),
                h5_dataset("incident_angle", "mirror/incident_angle", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_ANGLE", "degrees")
                }),
                h5_dataset("type", "mirror/type", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("substrate_material", "mirror/substrate_material", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("description", "mirror/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("coating_material", "mirror/coating_material", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("distance", "mirror/distance", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            }),

            h5_group("slit_4", {
                h5_attribute_val("NX_class", "NXslit"),

                h5_dataset("y_gap", "slit_4/y_gap", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("x_gap", "slit_4/x_gap", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("x_translation", "slit_4/x_translation", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("distance", "slit_4/distance", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("material", "slit_4/material", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("description", "slit_4/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("height", "slit_4/height", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            }),

            h5_group("beam_stop_1", {
                h5_attribute_val("NX_class", "NX_beamstop"),

                h5_dataset("x", "beam_stop_1/x", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("size", "beam_stop_1/size", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("distance_to_detector", "beam_stop_1/distance_to_detector", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("description", "beam_stop_1/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("y", "beam_stop_1/y", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("status", "beam_stop_1/status", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                })
            }),

            h5_group("XBPM5", {
                h5_attribute_val("NX_class", "NXdetector_group"),

                h5_dataset("group_parent", "XBPM5/group_parent", NX_INT, {
                    h5_attribute_val("NX_class", "NX_INT")
                }),
                h5_dataset("group_names", "XBPM5/group_names", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("group_index", "XBPM5/group_index", NX_INT, {
                    h5_attribute_val("NX_class", "NX_INT")
                }),

                h5_group("XBPM5_y", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("data", "XBPM5_y/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM5_y/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                }),

                h5_group("XBPM5_x", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("data", "XBPM5_x/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM5_x/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                }),

                h5_group("XBPM5", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("count_time", "XBPM5/count_time", NX_NUMBER, {
                        h5_attribute_val("NX_class", "NX_NUMBER"),
                        h5_attribute_val("NX_TIME", "s")
                    }),
                    h5_dataset("type", "XBPM5/type", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("distance", "XBPM5/distance", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("calibration_date", "XBPM5/calibration_date", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("description", "XBPM5/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("gain_setting", "XBPM5/gain_setting", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                }),

                h5_group("XBPM5_skew", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("description", "XBPM5_skew/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("data", "XBPM5_skew/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                }),

                h5_group("XBPM5_sum", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("saturation_value", "XBPM5_sum/saturation_value", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("data", "XBPM5_sum/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM5_sum/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                })
            }),

            h5_group("filter_set", {
                h5_attribute_val("NX_class", "NXattenuator"),

                h5_dataset("type", "filter_set/type", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("description", "filter_set/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("attenuator_transmission", "filter_set/attenuator_transmission", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("distance", "filter_set/distance", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            }),

            h5_group("slit_1", {
                h5_attribute_val("NX_class", "NXslit"),

                h5_dataset("distance", "slit_1/distance", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("height", "slit_1/height", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("x_gap", "slit_1/x_gap", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("y_gap", "slit_1/y_gap", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("material", "slit_1/material", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("x_translation", "slit_1/x_translation", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("description", "slit_1/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                })
            }),

            h5_group("XBPM6", {
                h5_attribute_val("NX_class", "NXdetector_group"),

                h5_dataset("group_parent", "XBPM6/group_parent", NX_INT, {
                    h5_attribute_val("NX_class", "NX_INT")
                }),
                h5_dataset("group_index", "XBPM6/group_index", NX_INT, {
                    h5_attribute_val("NX_class", "NX_INT")
                }),
                h5_dataset("group_names", "XBPM6/group_names", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),

                h5_group("XBPM6_y", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("data", "XBPM6_y/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM6_y/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                }),

                h5_group("XBPM6_x", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("description", "XBPM6_x/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("data", "XBPM6_x/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                }),

                h5_group("XBPM6", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("gain_setting", "XBPM6/gain_setting", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("count_time", "XBPM6/count_time", NX_NUMBER, {
                        h5_attribute_val("NX_class", "NX_NUMBER"),
                        h5_attribute_val("NX_TIME", "s")
                    }),
                    h5_dataset("calibration_date", "XBPM6/calibration_date", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("distance", "XBPM6/distance", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM6/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("type", "XBPM6/type", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                }),

                h5_group("XBPM6_skew", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("description", "XBPM6_skew/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("data", "XBPM6_skew/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                }),

                h5_group("XBPM6_sum", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("data", "XBPM6_sum/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("saturation_value", "XBPM6_sum/saturation_value", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM6_sum/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                })
            }),

            h5_group("insertion_device", {
                h5_attribute_val("NX_class", "NXinsertion_device"),

                h5_dataset("k", "insertion_device/k", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_DIMENSIONLESS", "True")
                }),
                h5_dataset("type", "insertion_device/type", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("gap", "insertion_device/gap", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("harmonic", "insertion_device/harmonic", NX_INT, {
                    h5_attribute_val("NX_UNITLESS", "true"),
                    h5_attribute_val("NX_class", "NX_INT")
                }),
                h5_dataset("length", "insertion_device/length", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            }),

            h5_group("slit_0", {
                h5_attribute_val("NX_class", "NXslit"),

                h5_dataset("x_gap", "slit_0/x_gap", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("distance", "slit_0/distance", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("description", "slit_0/description", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("x_translation", "slit_0/x_translation", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("material", "slit_0/material", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                })
            }),

            h5_group("monochromator", {
                h5_attribute_val("NX_class", "NXmonochromator"),

                h5_dataset("type", "monochromator/type", NXnote, {
                    h5_attribute_val("NX_class", "NXnote")
                }),
                h5_dataset("wavelength", "monochromator/wavelength", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_WAVELENGTH", "Angstrom")
                }),
                h5_dataset("distance", "monochromator/distance", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("wavelength_spread", "monochromator/wavelength_spread", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("energy", "monochromator/energy", NX_FLOAT, {
                    h5_attribute_val("NX_ENERGY", "keV"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),

                h5_group("crystal_1", {
                    h5_attribute_val("NX_class", "NXcrystal"),

                    h5_dataset("bragg_angle", "crystal_1/bragg_angle", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT"),
                        h5_attribute_val("NX_ANGLE", "degrees")
                    }),
                    h5_dataset("temperature", "crystal_1/temperature", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("order_no", "crystal_1/order_no", NX_INT, {
                        h5_attribute_val("NX_class", "NX_INT")
                    }),
                    h5_dataset("reflection", "crystal_1/reflection", NX_INT, {
                        h5_attribute_val("NX_class", "NX_INT"),
                        h5_attribute_val("NX_UNITLESS", "")
                    }),
                    h5_dataset("type", "crystal_1/type", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("usage", "crystal_1/usage", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                }),

                h5_group("crystal_2", {
                    h5_attribute_val("NX_class", "NXcrystal"),

                    h5_dataset("temperature", "crystal_2/temperature", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("order_no", "crystal_2/order_no", NX_INT, {
                        h5_attribute_val("NX_class", "NX_INT")
                    }),
                    h5_dataset("usage", "crystal_2/usage", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("bragg_angle", "crystal_2/bragg_angle", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT"),
                        h5_attribute_val("NX_ANGLE", "degrees")
                    }),
                    h5_dataset("type", "crystal_2/type", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("reflection", "crystal_2/reflection", NX_INT, {
                        h5_attribute_val("NX_UNITLESS", ""),
                        h5_attribute_val("NX_class", "NX_INT")
                    }),
                    h5_dataset("bend_x", "crystal_2/bend_x", NX_FLOAT, {
                        h5_attribute_val("NX_ANY", ""),
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                })
            }),

            h5_group("XBPM4", {
                h5_attribute_val("NX_class", "NXdetector_group"),

                h5_dataset("group_index", "XBPM4/group_index", NX_INT, {
                    h5_attribute_val("NX_class", "NX_INT")
                }),
                h5_dataset("group_parent", "XBPM4/group_parent", NX_INT, {
                    h5_attribute_val("NX_class", "NX_INT")
                }),
                h5_dataset("group_names", "XBPM4/group_names", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),

                h5_group("XBPM4_sum", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("data", "XBPM4_sum/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM4_sum/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("saturation_value", "XBPM4_sum/saturation_value", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                }),

                h5_group("XBPM4", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("type", "XBPM4/type", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("calibration_date", "XBPM4/calibration_date", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("description", "XBPM4/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("count_time", "XBPM4/count_time", NX_NUMBER, {
                        h5_attribute_val("NX_TIME", "s"),
                        h5_attribute_val("NX_class", "NX_NUMBER")
                    }),
                    h5_dataset("gain_setting", "XBPM4/gain_setting", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("distance", "XBPM4/distance", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                }),

                h5_group("XBPM4_skew", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("data", "XBPM4_skew/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM4_skew/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                }),

                h5_group("XBPM4_x", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("data", "XBPM4_x/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    }),
                    h5_dataset("description", "XBPM4_x/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    })
                }),

                h5_group("XBPM4_y", {
                    h5_attribute_val("NX_class", "NX_detector"),

                    h5_dataset("description", "XBPM4_y/description", NX_CHAR, {
                        h5_attribute_val("NX_class", "NX_CHAR")
                    }),
                    h5_dataset("data", "XBPM4_y/data", NX_FLOAT, {
                        h5_attribute_val("NX_class", "NX_FLOAT")
                    })
                })
            }),

            h5_group("source", {
                h5_attribute_val("NX_class", "NXsource"),

                h5_dataset("name", "source/name", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("distance", "source/distance", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("sigma_x", "source/sigma_x", NX_FLOAT, {
                    h5_attribute_val("NX_class", "NX_FLOAT"),
                    h5_attribute_val("NX_LENGTH", "mm")
                }),
                h5_dataset("probe", "source/probe", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("current", "source/current", NX_FLOAT, {
                    h5_attribute_val("NX_CURRENT", "mA"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("sigma_y", "source/sigma_y", NX_FLOAT, {
                    h5_attribute_val("NX_LENGTH", "mm"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("type", "source/type", NX_CHAR, {
                    h5_attribute_val("NX_class", "NX_CHAR")
                }),
                h5_dataset("divergence_x", "source/divergence_x", NX_FLOAT, {
                    h5_attribute_val("NX_ANGLE", "radians"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                }),
                h5_dataset("divergence_y", "source/divergence_y", NX_FLOAT, {
                    h5_attribute_val("NX_ANGLE", "radians"),
                    h5_attribute_val("NX_class", "NX_FLOAT")
                })
            })
        })
    });

    return format;
        
}

int main (int argc, char *argv[])
{
}