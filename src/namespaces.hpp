#ifndef NAMESPACES_HPP
#define NAMESPACES_HPP

#define CELLML_1_1_NS        "http://www.cellml.org/cellml/1.1#"
#define CELLML_1_0_NS        "http://www.cellml.org/cellml/1.0#"
#define CMETA_NS             "http://www.cellml.org/metadata/1.0#"
#define BQS_NS               "http://www.cellml.org/bqs/1.0#"
#define MATHML_NS            "http://www.w3.org/1998/Math/MathML"
#define RDF_NS               "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define RDF_DATATYPE_DOUBLE  "http://www.w3.org/2001/XMLSchema#double"
#define RDF_DATATYPE_BOOLEAN "http://www.w3.org/2001/XMLSchema#boolean"
#define RDF_DATATYPE_INTEGER "http://www.w3.org/2001/XMLSchema#integer"
#define BQMODEL_NS           "http://biomodels.net/model-qualifiers/"
#define BQBIO_NS             "http://biomodels.net/biology-qualifiers/"
#define DC_NS                "http://purl.org/dc/elements/1.1/"
#define DCTERMS_NS           "http://purl.org/dc/terms/"
#define VCARD_NS             "http://www.w3.org/2001/vcard-rdf/3.0#"
#define XML_NS               "http://www.w3.org/XML/1998/namespace"
#define OBO_RO_NS            "http://www.obofoundry.org/ro/ro.owl#"

#define CSIM_NS              "http://cellml.sourceforge.net/csim/metadata/0.1#"
#define CSIM_NS2             "http://cellml.sourceforge.net/ns"
#define CSIM_MODEL_TYPE_NS   CSIM_NS2 "/model-type"

#define MODEL_TYPE_SMALL_MOLECULE CSIM_MODEL_TYPE_NS "/small-molecule"
#define MODEL_TYPE_CELL           CSIM_MODEL_TYPE_NS "/cell"
#define MODEL_TYPE_TRANSPORTER    CSIM_MODEL_TYPE_NS "/transport-protein"

#define CSIM_SIMULATION_NS "http://cellml.sourceforge.net/csim/simulation/0.1#"
#define CSIM_SIMULATION_PREFIX "csim"
#define OMEX_MANIFEST_NS "http://identifiers.org/combine.specifications/omex-manifest"
#define OMEX_MANIFEST_PREFIX "omf"

#define CELLML_1_1_PREFIX "cellml"

#define ID_ORG_UNIPROT              "http://identifiers.org/uniprot/"
#define ID_ORG_FMA                  "http://identifiers.org/fma/"

#define UNIPROT_NAME                "UNIPROT"
#define FMA_NAME                    "FMA"

#endif // NAMESPACES_HPP
