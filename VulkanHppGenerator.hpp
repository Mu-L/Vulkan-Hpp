// Copyright(c) 2015-2019, NVIDIA CORPORATION. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "XMLHelper.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <tinyxml2.h>
#include <vector>

constexpr size_t INVALID_INDEX = static_cast<size_t>( ~0 );

template <typename BitType>
class Flags
{
public:
  using MaskType = typename std::underlying_type<BitType>::type;

  constexpr Flags() noexcept : m_mask( 0 ) {}

  constexpr Flags( BitType bit ) noexcept : m_mask( static_cast<MaskType>( bit ) ) {}

  constexpr explicit Flags( MaskType flags ) noexcept : m_mask( flags ) {}

  constexpr Flags<BitType> & operator|=( Flags<BitType> const & rhs ) noexcept
  {
    m_mask |= rhs.m_mask;
    return *this;
  }

  constexpr bool operator!() const noexcept
  {
    return !m_mask;
  }

  constexpr bool operator&( BitType const & rhs ) const noexcept
  {
    return m_mask & static_cast<MaskType>( rhs );
  }

  constexpr Flags<BitType> operator&( Flags<BitType> const & rhs ) const noexcept
  {
    return Flags<BitType>( m_mask & rhs.m_mask );
  }

  constexpr Flags<BitType> operator|( Flags<BitType> const & rhs ) const noexcept
  {
    return Flags<BitType>( m_mask | rhs.m_mask );
  }

private:
  MaskType m_mask;
};

enum class CommandFlavourFlagBits : uint8_t
{
  enhanced      = 1 << 0,
  chained       = 1 << 1,
  singular      = 1 << 2,
  unique        = 1 << 3,
  withAllocator = 1 << 4,
  noReturn      = 1 << 5,
  keepVoidPtr   = 1 << 6
};
using CommandFlavourFlags = Flags<CommandFlavourFlagBits>;

constexpr CommandFlavourFlags operator|( CommandFlavourFlagBits const & lhs, CommandFlavourFlagBits const & rhs ) noexcept
{
  return CommandFlavourFlags( lhs ) | CommandFlavourFlags( rhs );
}

class VulkanHppGenerator
{
public:
  VulkanHppGenerator( tinyxml2::XMLDocument const & document, std::string const & api );

  VulkanHppGenerator()                                             = delete;
  VulkanHppGenerator( VulkanHppGenerator const & rhs )             = delete;
  VulkanHppGenerator( VulkanHppGenerator && rhs )                  = delete;
  VulkanHppGenerator & operator=( VulkanHppGenerator const & rhs ) = delete;
  VulkanHppGenerator & operator=( VulkanHppGenerator && rhs )      = delete;

  void distributeSecondLevelCommands();
  void generateEnumsHppFile() const;
  void generateExtensionInspectionFile() const;
  void generateFormatTraitsHppFile() const;
  void generateFuncsHppFile() const;
  void generateHandlesHppFile() const;
  void generateHashHppFile() const;
  void generateHppFile() const;
  void generateMacrosFile() const;
  void generateRAIIHppFile() const;
  void generateSharedHppFile() const;
  void generateStaticAssertionsHppFile() const;
  void generateStructsHppFile() const;
  void generateToStringHppFile() const;
  void generateCppModuleFile() const;
  void prepareRAIIHandles();

  struct MacroData
  {
    std::string              deprecatedComment = {};
    std::string              calleeMacro       = {};
    std::vector<std::string> params            = {};
    std::string              definition        = {};
  };

private:
  struct NameLine
  {
    std::string name    = {};
    int         xmlLine = {};
  };

  struct BaseTypeData
  {
    TypeInfo typeInfo = {};
    int      xmlLine  = {};
  };

  struct BitmaskData
  {
    std::map<std::string, int> aliases = {};
    std::string                require = {};
    std::string                type    = {};
    int                        xmlLine = {};
  };

  struct EnumValueAlias
  {
    std::string alias     = {};
    std::string name      = {};
    std::string protect   = {};
    bool        supported = {};
    int         xmlLine   = {};
  };

  struct EnumValueData
  {
    std::vector<EnumValueAlias> aliases    = {};
    std::string                 bitpos     = {};
    bool                        deprecated = {};
    std::string                 name       = {};
    std::string                 protect    = {};
    bool                        supported  = {};
    std::string                 value      = {};
    int                         xmlLine    = {};
  };

  struct EnumData
  {
    bool addEnumAlias( int line, std::string const & name, std::string const & alias, std::string const & protect, bool supported );
    bool addEnumValue( int                 line,
                       std::string const & valueName,
                       std::string const & protect,
                       std::string const & bitpos,
                       std::string const & value,
                       bool                supported,
                       bool                deprecated );

    std::map<std::string, int>  aliases      = {};
    std::string                 bitwidth     = {};
    bool                        isBitmask    = false;
    std::vector<EnumValueAlias> valueAliases = {};  // temporary storage for aliases, as they might be specified before the actual value is specified
    std::vector<EnumValueData>  values       = {};
    int                         xmlLine      = {};
  };

  struct EnumExtendData
  {
    std::string           alias      = {};
    std::string           api        = {};
    std::string           name       = {};
    std::string           protect    = {};
    std::set<std::string> requiredBy = {};
    bool                  supported  = {};
    int                   xmlLine    = {};
  };

  struct NameData
  {
    std::string              name       = {};
    std::vector<std::string> arraySizes = {};
  };

  struct ParamData
  {
    TypeInfo                                    type          = {};
    std::string                                 name          = {};
    std::vector<std::string>                    arraySizes    = {};
    std::string                                 lenExpression = {};
    std::vector<std::pair<std::string, size_t>> lenParams     = {};
    bool                                        optional      = false;
    std::pair<std::string, size_t>              strideParam   = {};
    int                                         xmlLine       = {};
  };

  struct CommandData
  {
    std::map<std::string, int> aliases      = {};
    std::vector<std::string>   errorCodes   = {};
    std::vector<std::string>   exports      = {};
    std::string                handle       = {};
    std::vector<ParamData>     params       = {};
    std::set<std::string>      requiredBy   = {};
    std::string                returnType   = {};
    std::vector<std::string>   successCodes = {};
    int                        xmlLine      = {};
  };

  struct ConstantData
  {
    std::string type    = {};
    std::string value   = {};
    int         xmlLine = {};
  };

  struct DefineData
  {
    bool                     deprecated         = false;
    std::string              require            = {};
    int                      xmlLine            = {};
    std::string              deprecationReason  = {};
    std::string              possibleCallee     = {};
    std::vector<std::string> params             = {};
    std::string              possibleDefinition = {};
  };

  struct DefinesPartition
  {
    std::map<std::string, DefineData> callees = {};
    std::map<std::string, DefineData> callers = {};
    std::map<std::string, DefineData> values  = {};
  };

  struct RequireFeature
  {
    std::vector<std::string> name      = {};
    std::string              structure = {};
    int                      xmlLine   = {};
  };

  struct RemoveData
  {
    std::vector<std::string>    commands = {};
    std::vector<std::string>    enums    = {};
    std::vector<RequireFeature> features = {};
    std::vector<std::string>    types    = {};
    int                         xmlLine  = {};
  };

  struct RequireData
  {
    std::string                        api           = {};
    std::string                        depends       = {};
    std::vector<NameLine>              commands      = {};
    std::map<std::string, std::string> enumConstants = {};
    std::vector<std::string>           constants     = {};
    std::vector<RequireFeature>        features      = {};
    std::vector<NameLine>              types         = {};
    int                                xmlLine       = {};
  };

  struct DeprecateData
  {
    std::string              explanationLink = {};
    std::vector<std::string> commands        = {};
    std::vector<std::string> types           = {};
    int                      xmlLine         = 0;
  };

  struct ExtensionData
  {
    std::string                                               deprecatedBy           = {};
    bool                                                      isDeprecated           = false;
    std::string                                               name                   = {};
    std::string                                               number                 = {};
    std::string                                               obsoletedBy            = {};
    std::string                                               platform               = {};
    std::string                                               promotedTo             = {};
    std::map<std::string, std::vector<std::set<std::string>>> depends                = {};
    std::vector<std::string>                                  ratified               = {};
    std::vector<DeprecateData>                                deprecateData          = {};
    std::vector<RemoveData>                                   removeData             = {};
    std::vector<RequireData>                                  requireData            = {};
    std::vector<std::string>                                  supported              = {};
    std::string                                               type                   = {};
    std::vector<RequireData>                                  unsupportedRequireData = {};
    int                                                       xmlLine                = 0;
  };

  struct FeatureData
  {
    std::vector<std::string>   api           = {};
    std::string                name          = {};
    std::string                number        = {};
    std::vector<DeprecateData> deprecateData = {};
    std::vector<RemoveData>    removeData    = {};
    std::vector<RequireData>   requireData   = {};
    int                        xmlLine       = 0;
  };

  struct ExternalTypeData
  {
    std::string require = {};
    int         xmlLine = 0;
  };

  struct ComponentData
  {
    std::string bits          = {};
    std::string name          = {};
    std::string numericFormat = {};
    std::string planeIndex    = {};
    int         xmlLine       = {};
  };

  struct PlaneData
  {
    std::string compatible    = {};
    std::string heightDivisor = {};
    std::string widthDivisor  = {};
    int         xmlLine       = {};
  };

  struct FormatData
  {
    std::string                blockExtent      = {};
    std::string                blockSize        = {};
    std::string                chroma           = {};
    std::string                classAttribute   = {};
    std::vector<ComponentData> components       = {};
    std::string                compressed       = {};
    std::string                packed           = {};
    std::vector<PlaneData>     planes           = {};
    std::string                spirvImageFormat = {};
    std::string                texelsPerBlock   = {};
    int                        xmlLine          = {};
  };

  struct FuncPointerArgumentData
  {
    std::string name    = {};
    TypeInfo    type    = {};
    int         xmlLine = {};
  };

  struct FuncPointerData
  {
    std::vector<FuncPointerArgumentData> arguments  = {};
    std::string                          require    = {};
    TypeInfo                             returnType = {};
    int                                  xmlLine    = {};
  };

  struct HandleData
  {
    std::map<std::string, int> aliases             = {};
    std::set<std::string>      childrenHandles     = {};
    std::set<std::string>      commands            = {};
    std::string                deleteCommand       = {};
    std::string                deletePool          = {};
    std::string                destructorType      = {};
    std::string                objTypeEnum         = {};
    std::string                parent              = {};
    std::set<std::string>      secondLevelCommands = {};
    bool                       isDispatchable      = {};
    int                        xmlLine             = {};

    // RAII data
    std::map<std::string, CommandData>::const_iterator              destructorIt   = {};
    std::vector<std::map<std::string, CommandData>::const_iterator> constructorIts = {};
  };

  struct PlatformData
  {
    std::string protect = {};
    int         xmlLine = {};
  };

  struct MemberData
  {
    std::string                                 defaultValue   = {};
    TypeInfo                                    type           = {};
    std::string                                 name           = {};
    std::vector<std::string>                    arraySizes     = {};
    std::string                                 bitCount       = {};
    std::string                                 deprecated     = {};
    std::vector<std::string>                    lenExpressions = {};
    std::vector<std::pair<std::string, size_t>> lenMembers     = {};
    bool                                        noAutoValidity = {};
    std::vector<bool>                           optional       = {};
    std::vector<std::string>                    selection      = {};
    std::string                                 selector       = {};
    std::string                                 value          = {};
    int                                         xmlLine        = {};
  };

  struct SpirVCapabilityData
  {
    std::map<std::string, std::map<std::string, int>> structs = {};  // map from structure to map from member to xmlLine
    int                                               xmlLine = {};
  };

  struct StructureData
  {
    std::map<std::string, int> aliases             = {};
    bool                       allowDuplicate      = {};
    bool                       isExtended          = {};
    bool                       isUnion             = {};
    bool                       returnedOnly        = {};
    bool                       mutualExclusiveLens = {};
    std::vector<MemberData>    members             = {};
    std::vector<std::string>   structExtends       = {};
    std::string                subStruct           = {};
    int                        xmlLine             = {};
  };

  struct TagData
  {
    int xmlLine = {};
  };

  struct VectorParamData
  {
    size_t lenParam    = INVALID_INDEX;
    size_t strideParam = INVALID_INDEX;
    bool   byStructure = false;
  };

  struct VideoRequireCapabilities
  {
    int         xmlLine = {};
    std::string name;
    std::string member;
    std::string value;
  };

  struct VideoFormat
  {
    int                                   xmlLine = {};
    std::vector<std::string>              formatProperties;
    std::string                           name;
    std::vector<std::string>              usage;
    std::vector<VideoRequireCapabilities> requireCapabilities;
  };

  struct VideoProfile
  {
    int         xmlLine = {};
    std::string name;
    std::string value;
  };

  struct VideoProfileMember
  {
    int                       xmlLine = {};
    std::string               name;
    std::vector<VideoProfile> profiles;
  };

  struct VideoProfiles
  {
    int                             xmlLine = {};
    std::string                     name;
    std::vector<VideoProfileMember> members;
  };

  struct VideoCodec
  {
    int                        xmlLine = {};
    std::string                name;
    std::vector<std::string>   capabilities;
    std::string                extend;
    std::string                value;
    std::vector<VideoFormat>   formats;
    std::vector<VideoProfiles> profiles;
  };

  struct MacroVisitor final : tinyxml2::XMLVisitor
  {
    // comments, then name, then parameters and definition together, because that's how they appear in the xml!
    // guaranteed to be 3 elements long
    std::vector<std::string> macro;

    bool Visit( tinyxml2::XMLText const & text ) override
    {
      if ( auto const nodeText = text.Value(); nodeText != nullptr )
      {
        macro.emplace_back( nodeText );
      }
      return true;
    }
  };

private:
  void        addCommand( std::string const & name, CommandData & commandData );
  void        addCommandsToHandle( std::vector<RequireData> const & requireData );
  void        addMissingFlagBits( std::vector<RequireData> & requireData, std::string const & requiredBy );
  std::string addTitleAndProtection( std::string const & title, std::string const & strIf, std::string const & strElse = {} ) const;
  bool        allVectorSizesSupported( std::vector<ParamData> const & params, std::map<size_t, VectorParamData> const & vectorParams ) const;
  void        appendDispatchLoaderDynamicCommands( std::vector<RequireData> const & requireData,
                                                   std::set<std::string> &          listedCommands,
                                                   std::string const &              title,
                                                   std::string &                    commandMembers,
                                                   std::string &                    initialCommandAssignments,
                                                   std::string &                    instanceCommandAssignments,
                                                   std::string &                    deviceCommandAssignments ) const;
  void        appendCppModuleCommands( std::vector<RequireData> const & requireData,
                                       std::set<std::string> &          listedCommands,
                                       std::string const &              title,
                                       std::string &                    commandMembers ) const;
  void        appendRAIIDispatcherCommands( std::vector<RequireData> const & requireData,
                                            std::set<std::string> &          listedCommands,
                                            std::string const &              title,
                                            std::string &                    contextInitializers,
                                            std::string &                    contextMembers,
                                            std::string &                    deviceAssignments,
                                            std::string &                    deviceMembers,
                                            std::string &                    instanceAssignments,
                                            std::string &                    instanceMembers ) const;
  void        checkAttributes( int                                                  line,
                               std::map<std::string, std::string> const &           attributes,
                               std::map<std::string, std::set<std::string>> const & required,
                               std::map<std::string, std::set<std::string>> const & optional = {} ) const;
  void        checkBitmaskCorrectness() const;
  void        checkCommandCorrectness() const;
  void        checkCorrectness() const;
  void        checkDefineCorrectness() const;
  void        checkElements( int                                               line,
                             std::vector<tinyxml2::XMLElement const *> const & elements,
                             std::map<std::string, bool> const &               required,
                             std::set<std::string> const &                     optional = {} ) const;
  void        checkEnumCorrectness() const;
  bool        checkEquivalentSingularConstructor( std::vector<std::map<std::string, CommandData>::const_iterator> const & constructorIts,
                                                  std::map<std::string, CommandData>::const_iterator                      constructorIt,
                                                  std::vector<ParamData>::const_iterator                                  lenIt ) const;
  void        checkExtensionCorrectness() const;
  void        checkForError( bool condition, int line, std::string const & message ) const;
  void        checkForWarning( bool condition, int line, std::string const & message ) const;
  void        checkFuncPointerCorrectness() const;
  void        checkHandleCorrectness() const;
  void        checkRequireCorrectness() const;
  void        checkRequireCorrectness( std::vector<RequireData> const & requireData, std::string const & section, std::string const & name ) const;
  void        checkRequireDependenciesCorrectness( RequireData const & require, std::string const & section, std::string const & name ) const;
  void        checkRequireTypesCorrectness( RequireData const & require ) const;
  void        checkSpirVCapabilityCorrectness() const;
  void        checkStructCorrectness() const;
  void checkStructMemberCorrectness( std::string const & structureName, std::vector<MemberData> const & members, std::set<std::string> & sTypeValues ) const;
  void checkSyncAccessCorrectness() const;
  void checkSyncStageCorrectness() const;
  std::string              combineDataTypes( std::map<size_t, VectorParamData> const & vectorParams,
                                             std::vector<size_t> const &               returnParams,
                                             bool                                      enumerating,
                                             std::vector<std::string> const &          dataTypes,
                                             CommandFlavourFlags                       flavourFlags,
                                             bool                                      raii ) const;
  bool                     contains( std::vector<EnumValueData> const & enumValues, std::string const & name ) const;
  bool                     containsArray( std::string const & type ) const;
  bool                     containsDeprecated( std::vector<MemberData> const & members ) const;
  bool                     containsFuncPointer( std::string const & type ) const;
  bool                     containsFloatingPoints( std::vector<MemberData> const & members ) const;
  bool                     containsUnion( std::string const & type ) const;
  bool                     describesVector( StructureData const & structure, std::string const & type = "" ) const;
  std::vector<size_t>      determineChainedReturnParams( std::vector<ParamData> const & params, std::vector<size_t> const & returnParams ) const;
  std::vector<size_t>      determineConstPointerParams( std::vector<ParamData> const & params ) const;
  std::vector<std::string> determineDataTypes( std::vector<VulkanHppGenerator::ParamData> const & params,
                                               std::map<size_t, VectorParamData> const &          vectorParams,
                                               std::vector<size_t> const &                        returnParams,
                                               std::set<size_t> const &                           templatedParams,
                                               bool                                               raii ) const;
  size_t                   determineDefaultStartIndex( std::vector<ParamData> const & params, std::set<size_t> const & skippedParams ) const;
  bool                     determineEnumeration( std::map<size_t, VectorParamData> const & vectorParams, std::vector<size_t> const & returnParams ) const;
  size_t                   determineInitialSkipCount( std::string const & command ) const;
  std::vector<size_t>      determineReturnParams( std::vector<ParamData> const & params ) const;
  std::vector<std::map<std::string, CommandData>::const_iterator>
    determineRAIIHandleConstructors( std::string const & handleType, std::map<std::string, CommandData>::const_iterator destructorIt ) const;
  std::map<std::string, CommandData>::const_iterator determineRAIIHandleDestructor( std::string const & handleType ) const;
  std::set<size_t>                                determineSingularParams( size_t returnParam, std::map<size_t, VectorParamData> const & vectorParams ) const;
  std::set<size_t>                                determineSkippedParams( std::vector<ParamData> const &            params,
                                                                          size_t                                    initialSkipCount,
                                                                          std::map<size_t, VectorParamData> const & vectorParams,
                                                                          std::vector<size_t> const &               returnParam,
                                                                          bool                                      singular ) const;
  std::string                                     determineSubStruct( std::pair<std::string, StructureData> const & structure ) const;
  std::map<size_t, VectorParamData>               determineVectorParams( std::vector<ParamData> const & params ) const;
  std::set<size_t>                                determineVoidPointerParams( std::vector<ParamData> const & params ) const;
  void                                            distributeEnumExtends();
  void                                            distributeEnumValueAliases();
  void                                            distributeRequirements();
  void                                            distributeRequirements( std::vector<RequireData> const & requireData, std::string const & requiredBy );
  void                                            distributeStructAliases();
  void                                            filterLenMembers();
  std::map<std::string, NameLine>::const_iterator findAlias( std::string const & name, std::map<std::string, NameLine> const & aliases ) const;
  std::string                                     findBaseName( std::string aliasName, std::map<std::string, NameLine> const & aliases ) const;
  std::vector<FeatureData>::const_iterator        findFeature( std::string const & name ) const;
  std::vector<ParamData>::const_iterator          findParamIt( std::string const & name, std::vector<ParamData> const & paramData ) const;
  std::vector<MemberData>::const_iterator         findStructMemberIt( std::string const & name, std::vector<MemberData> const & memberData ) const;
  std::vector<MemberData>::const_iterator         findStructMemberItByType( std::string const & type, std::vector<MemberData> const & memberData ) const;
  std::vector<ExtensionData>::const_iterator      findSupportedExtension( std::string const & name ) const;
  std::string                                     findTag( std::string const & name, std::string const & postfix = "" ) const;
  std::set<std::string>                           gatherResultCodes() const;
  std::pair<std::string, std::string>             generateAllocatorTemplates( std::vector<size_t> const &               returnParams,
                                                                              std::vector<std::string> const &          returnDataTypes,
                                                                              std::map<size_t, VectorParamData> const & vectorParams,
                                                                              std::vector<size_t> const &               chainedReturnParams,
                                                                              CommandFlavourFlags                       flavourFlags,
                                                                              bool                                      definition ) const;
  std::string                                     generateArgumentListEnhanced( std::vector<ParamData> const &            params,
                                                                                std::vector<size_t> const &               returnParams,
                                                                                std::map<size_t, VectorParamData> const & vectorParams,
                                                                                std::set<size_t> const &                  skippedParams,
                                                                                std::set<size_t> const &                  singularParams,
                                                                                std::set<size_t> const &                  templatedParams,
                                                                                std::vector<size_t> const &               chainedReturnParams,
                                                                                bool                                      raii,
                                                                                bool                                      definition,
                                                                                CommandFlavourFlags                       flavourFlags,
                                                                                bool                                      withDispatcher ) const;
  std::string
    generateArgumentListStandard( std::vector<ParamData> const & params, std::set<size_t> const & skippedParams, bool definition, bool withDispatcher ) const;
  std::string generateArgumentTemplates( std::vector<ParamData> const &            params,
                                         std::vector<size_t> const &               returnParams,
                                         std::map<size_t, VectorParamData> const & vectorParams,
                                         std::set<size_t> const &                  templatedParams,
                                         std::vector<size_t> const &               chainedReturnParams,
                                         bool                                      raii ) const;
  std::string generateBaseTypes() const;
  std::string generateBitmask( std::map<std::string, BitmaskData>::const_iterator bitmaskIt, std::string const & surroundingProtect ) const;
  std::string generateBitmasksToString() const;
  std::string generateBitmasksToString( std::vector<RequireData> const & requireData, std::set<std::string> & listedBitmasks, std::string const & title ) const;
  std::string generateBitmaskToString( std::map<std::string, BitmaskData>::const_iterator bitmaskIt ) const;
  std::string generateCallArgumentsEnhanced( CommandData const &      commandData,
                                             size_t                   initialSkipCount,
                                             bool                     nonConstPointerAsNullptr,
                                             std::set<size_t> const & singularParams,
                                             std::set<size_t> const & templatedParams,
                                             bool                     raii,
                                             bool                     raiiFactory,
                                             CommandFlavourFlags      flavourFlags ) const;
  std::string generateCallArgumentsStandard( std::vector<ParamData> const & params, size_t initialSkipCount ) const;
  std::string generateCallArgumentEnhanced( std::vector<ParamData> const & params,
                                            size_t                         paramIndex,
                                            bool                           nonConstPointerAsNullptr,
                                            std::set<size_t> const &       singularParams,
                                            std::set<size_t> const &       templatedParams,
                                            CommandFlavourFlags            flavourFlags,
                                            bool                           raiiFactory ) const;
  std::string generateCallArgumentEnhancedConstPointer( ParamData const &        param,
                                                        size_t                   paramIndex,
                                                        std::set<size_t> const & singularParams,
                                                        std::set<size_t> const & templatedParams ) const;
  std::string generateCallArgumentEnhancedNonConstPointer( ParamData const &        param,
                                                           size_t                   paramIndex,
                                                           bool                     nonConstPointerAsNullptr,
                                                           std::set<size_t> const & singularParams ) const;
  std::string generateCallArgumentEnhancedValue( std::vector<ParamData> const & params,
                                                 size_t                         paramIndex,
                                                 std::set<size_t> const &       singularParams,
                                                 CommandFlavourFlags            flavourFlags,
                                                 bool                           raiiFactory ) const;
  std::string generateCallSequence( std::string const &                       name,
                                    CommandData const &                       commandData,
                                    std::vector<size_t> const &               returnParams,
                                    std::map<size_t, VectorParamData> const & vectorParams,
                                    size_t                                    initialSkipCount,
                                    std::set<size_t> const &                  singularParams,
                                    std::set<size_t> const &                  templatedParams,
                                    std::vector<size_t> const &               chainedReturnParams,
                                    CommandFlavourFlags                       flavourFlags,
                                    bool                                      raii,
                                    bool                                      raiiFactory ) const;
  std::string generateChainTemplates( std::vector<size_t> const & returnParams, bool chained ) const;
  std::string generateCommand( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandDefinitions() const;
  std::string
    generateCommandDefinitions( std::vector<RequireData> const & requireData, std::set<std::string> & listedCommands, std::string const & title ) const;
  std::string generateCommandDefinitions( std::string const & command, std::string const & handle ) const;
  std::string generateCommandEnhanced( std::string const &                       name,
                                       CommandData const &                       commandData,
                                       size_t                                    initialSkipCount,
                                       bool                                      definition,
                                       std::map<size_t, VectorParamData> const & vectorParams,
                                       std::vector<size_t> const &               returnParams,
                                       CommandFlavourFlags                       flavourFlags ) const;
  std::string generateCommandName( std::string const &            vulkanCommandName,
                                   std::vector<ParamData> const & params,
                                   size_t                         initialSkipCount,
                                   CommandFlavourFlags            flavourFlags = {} ) const;
  std::string generateCommandResult( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandResultMultiSuccessNoErrors(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandResultMultiSuccessWithErrors(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandResultMultiSuccessWithErrors1Return(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, size_t returnParam, bool raii ) const;
  std::string generateCommandResultMultiSuccessWithErrors2Return( std::string const &         name,
                                                                  CommandData const &         commandData,
                                                                  size_t                      initialSkipCount,
                                                                  bool                        definition,
                                                                  std::vector<size_t> const & returnParamIndices,
                                                                  bool                        raii ) const;
  std::string generateCommandResultMultiSuccessWithErrors3Return( std::string const &         name,
                                                                  CommandData const &         commandData,
                                                                  size_t                      initialSkipCount,
                                                                  bool                        definition,
                                                                  std::vector<size_t> const & returnParamIndices,
                                                                  bool                        raii ) const;
  std::string
    generateCommandResultSingleSuccess( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandResultSingleSuccessNoErrors(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors1Return(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, size_t returnParam, bool raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors1ReturnChain(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, size_t returnParam, bool raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors1ReturnHandle(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, size_t returnParam, bool raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors1ReturnValue(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, size_t returnParam, bool raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors1ReturnVoid(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, size_t returnParam, bool raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors2Return( std::string const &         name,
                                                                   CommandData const &         commandData,
                                                                   size_t                      initialSkipCount,
                                                                   bool                        definition,
                                                                   std::vector<size_t> const & returnParamIndices,
                                                                   bool                        raii ) const;
  std::string generateCommandResultSingleSuccessWithErrors3Return( std::string const &         name,
                                                                   CommandData const &         commandData,
                                                                   size_t                      initialSkipCount,
                                                                   bool                        definition,
                                                                   std::vector<size_t> const & returnParamIndices,
                                                                   bool                        raii ) const;
  std::string generateCommandResultWithErrors0Return(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandSet( bool                             definition,
                                  std::string const &              standard,
                                  std::vector<std::string> const & enhanced = {},
                                  std::vector<std::string> const & unique   = {} ) const;
  std::string generateCommandSet( std::string const & standard, std::string const & enhanced ) const;
  std::string generateCommandSetInclusive( std::string const &                      name,
                                           CommandData const &                      commandData,
                                           size_t                                   initialSkipCount,
                                           bool                                     definition,
                                           std::vector<size_t>                      returnParams,
                                           std::map<size_t, VectorParamData>        vectorParams,
                                           bool                                     unique,
                                           std::vector<CommandFlavourFlags> const & flags,
                                           bool                                     raii,
                                           bool                                     raiiFactory,
                                           std::vector<CommandFlavourFlags> const & raiiFlags ) const;
  std::string
    generateCommandSetExclusive( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandStandard( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition ) const;
  std::string generateCommandVoid( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandValue( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string
    generateCommandVoid0Return( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, bool raii ) const;
  std::string generateCommandVoid1Return(
    std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition, size_t returnParam, bool raii ) const;
  std::string generateCommandVoid2Return( std::string const &         name,
                                          CommandData const &         commandData,
                                          size_t                      initialSkipCount,
                                          bool                        definition,
                                          std::vector<size_t> const & returnParamIndices,
                                          bool                        raii ) const;
  std::string generateConstexprString( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateConstexprDefines() const;
  std::string generateConstexprUsings() const;
  std::string generateCppModuleFuncpointerUsings() const;
  std::string generateCppModuleHandleUsings() const;
  std::string generateCppModuleStructUsings() const;
  std::string generateCppModuleUniqueHandleUsings() const;
  std::string generateCppModuleFuncsUsings() const;
  std::string generateCppModuleEnumUsings() const;
  std::string generateCppModuleFormatTraitsUsings() const;
  std::string generateCppModuleExtensionInspectionUsings() const;
  std::string generateCppModuleUsings() const;
  std::string generateCppModuleCommands() const;
  std::string generateCppModuleRaiiUsings() const;
  std::string generateCppModuleSharedHandleUsings() const;
  std::string generateCppModuleHandleHashSpecializations() const;
  std::string generateCppModuleHashSpecializations() const;
  std::string generateCppModuleStructHashSpecializations() const;
  std::string generateDataDeclarations( CommandData const &                       commandData,
                                        std::vector<size_t> const &               returnParams,
                                        std::map<size_t, VectorParamData> const & vectorParams,
                                        std::set<size_t> const &                  templatedParams,
                                        CommandFlavourFlags                       flavourFlags,
                                        bool                                      raii,
                                        std::vector<std::string> const &          dataTypes,
                                        std::string const &                       dataType,
                                        std::string const &                       returnType,
                                        std::string const &                       returnVariable ) const;
  std::string generateDataDeclarations1Return( CommandData const &                       commandData,
                                               std::vector<size_t> const &               returnParams,
                                               std::map<size_t, VectorParamData> const & vectorParams,
                                               std::set<size_t> const &                  templatedParams,
                                               CommandFlavourFlags                       flavourFlags,
                                               std::vector<std::string> const &          dataTypes,
                                               std::string const &                       dataType,
                                               std::string const &                       returnType,
                                               std::string const &                       returnVariable ) const;
  std::string generateDataDeclarations2Returns( CommandData const &                       commandData,
                                                std::vector<size_t> const &               returnParams,
                                                std::map<size_t, VectorParamData> const & vectorParams,
                                                CommandFlavourFlags                       flavourFlags,
                                                bool                                      raii,
                                                std::vector<std::string> const &          dataTypes,
                                                std::string const &                       returnType,
                                                std::string const &                       returnVariable ) const;
  std::string generateDataDeclarations3Returns( CommandData const &                       commandData,
                                                std::vector<size_t> const &               returnParams,
                                                std::map<size_t, VectorParamData> const & vectorParams,
                                                CommandFlavourFlags                       flavourFlags,
                                                bool                                      raii,
                                                std::vector<std::string> const &          dataTypes ) const;
  std::string generateDataPreparation( CommandData const &                       commandData,
                                       size_t                                    initialSkipCount,
                                       std::vector<size_t> const &               returnParams,
                                       std::map<size_t, VectorParamData> const & vectorParams,
                                       std::set<size_t> const &                  templatedParams,
                                       CommandFlavourFlags                       flavourFlags,
                                       bool                                      enumerating,
                                       std::vector<std::string> const &          dataTypes ) const;
  std::string generateDataSizeChecks( CommandData const &                       commandData,
                                      std::vector<size_t> const &               returnParams,
                                      std::vector<std::string> const &          returnParamTypes,
                                      std::map<size_t, VectorParamData> const & vectorParams,
                                      std::set<size_t> const &                  templatedParams,
                                      bool                                      singular ) const;
  std::string generateDebugReportObjectType( std::string const & objectType ) const;
  std::string generateDecoratedReturnType( CommandData const &                       commandData,
                                           std::vector<size_t> const &               returnParams,
                                           std::map<size_t, VectorParamData> const & vectorParams,
                                           bool                                      enumerating,
                                           CommandFlavourFlags                       flavourFlags,
                                           std::string const &                       returnType ) const;
  std::string generateDeprecatedConstructors( std::string const & name ) const;
  std::string generateDeprecatedStructSetters( std::string const & name ) const;
  std::string generateDispatchLoaderDynamic() const;  // uses vkGet*ProcAddress to get function pointers
  std::string generateDispatchLoaderStatic() const;   // uses exported symbols from loader
  std::string generateDestroyCommand( std::string const & name, CommandData const & commandData ) const;
  std::string
    generateDispatchLoaderDynamicCommandAssignment( std::string const & commandName, std::string const & aliasName, std::string const & firstArg ) const;
  std::string generateDispatchLoaderStaticCommands( std::vector<RequireData> const & requireData,
                                                    std::set<std::string> &          listedCommands,
                                                    std::string const &              title ) const;
  std::string generateEnum( std::pair<std::string, EnumData> const & enumData, std::string const & surroundingProtect ) const;
  std::string generateEnumInitializer( TypeInfo const &                   type,
                                       std::vector<std::string> const &   arraySizes,
                                       std::vector<EnumValueData> const & values,
                                       bool                               bitmask ) const;
  std::string generateEnums() const;
  std::string generateEnums( std::vector<RequireData> const & requireData, std::set<std::string> & listedEnums, std::string const & title ) const;
  std::string generateEnumsToString() const;
  std::string generateEnumsToString( std::vector<RequireData> const & requireData, std::set<std::string> & listedEnums, std::string const & title ) const;
  std::string generateEnumToString( std::pair<std::string, EnumData> const & enumData ) const;
  std::pair<std::string, std::string> generateEnumSuffixes( std::string const & name, bool bitmask ) const;
  std::string                         generateEnumValueName( std::string const & enumName, std::string const & valueName, bool bitmask ) const;
  std::string                         generateExtensionDependencies() const;
  template <class Predicate, class Extraction>
  std::string generateExtensionReplacedBy( Predicate p, Extraction e ) const;
  template <class Predicate>
  std::string generateExtensionReplacedTest( Predicate p ) const;
  std::string generateExtensionsList( std::string const & type ) const;
  std::string generateExtensionTypeTest( std::string const & type ) const;
  std::string generateFormatTraits() const;
  std::string generateFormatTraitsCases( EnumData const &                                 enumData,
                                         std::function<bool( FormatData const & )>        predicate,
                                         std::function<std::string( FormatData const & )> generator ) const;
  std::string generateFormatTraitsList( EnumData const & enumData, std::function<bool( FormatData const & )> predicate ) const;
  template <typename T>
  std::string generateFormatTraitsSubCases( FormatData const &                                          formatData,
                                            std::function<std::vector<T> const &( FormatData const & )> accessor,
                                            std::string const &                                         subCaseName,
                                            std::function<std::string( T const & subCaseData )>         generator,
                                            std::string const &                                         defaultReturn ) const;
  std::string generateFuncPointer( std::pair<std::string, FuncPointerData> const & funcPointer, std::set<std::string> & listedStructs ) const;
  std::string generateFuncPointerReturns() const;
  std::string generateFunctionPointerCheck( std::string const & function, std::set<std::string> const & requiredBy, bool raii ) const;
  std::string generateHandle( std::pair<std::string, HandleData> const & handle, std::set<std::string> & listedHandles ) const;
  std::string generateHandleCommandDeclarations( std::set<std::string> const & commands ) const;
  std::string generateHandleDependencies( std::pair<std::string, HandleData> const & handle, std::set<std::string> & listedHandles ) const;
  std::string generateHandleEmpty( HandleData const & handleData ) const;
  std::string generateHandleForwardDeclarations() const;
  std::string generateHandleForwardDeclarations( std::vector<RequireData> const & requireData, std::string const & title ) const;
  std::string
    generateHandleHashStructures( std::vector<RequireData> const & requireData, std::string const & title, std::set<std::string> & listedHandles ) const;
  std::string generateHandleHashStructures() const;
  std::string generateHandles() const;
  std::string generateIndexTypeTraits( std::pair<std::string, EnumData> const & enumData ) const;
  std::string generateLayerSettingTypeTraits() const;
  std::string
              generateLenInitializer( std::vector<MemberData>::const_iterator                                                                                 mit,
                                      std::map<std::vector<MemberData>::const_iterator, std::vector<std::vector<MemberData>::const_iterator>>::const_iterator litit,
                                      bool mutualExclusiveLens ) const;
  std::string generateName( TypeInfo const & typeInfo ) const;
  std::string generateNoExcept( std::vector<std::string> const &          errorCodes,
                                std::vector<size_t> const &               returnParams,
                                std::map<size_t, VectorParamData> const & vectorParams,
                                CommandFlavourFlags                       flavourFlags,
                                bool                                      vectorSizeCheck,
                                bool                                      raii ) const;
  std::string generateObjectDeleter( std::string const & commandName, CommandData const & commandData, size_t initialSkipCount, size_t returnParam ) const;
  std::string generateObjectTypeToDebugReportObjectType() const;
  std::pair<std::string, std::string> generateProtection( std::string const & protect, bool defined = true ) const;
  std::string                         generateRAIICommandDefinitions() const;
  std::string
    generateRAIICommandDefinitions( std::vector<RequireData> const & requireData, std::set<std::string> & listedCommands, std::string const & title ) const;
  std::string generateRAIIDispatchers() const;
  std::string generateRAIIFactoryReturnStatements( CommandData const & commandData,
                                                   std::string const & vkType,
                                                   bool                enumerating,
                                                   std::string const & returnType,
                                                   std::string const & returnVariable,
                                                   bool                singular ) const;
  std::string generateRAIIHandle( std::pair<std::string, HandleData> const & handle,
                                  std::set<std::string> &                    listedHandles,
                                  std::set<std::string> const &              specialFunctions ) const;
  std::string generateRAIIHandleCommand( std::string const & command, size_t initialSkipCount, bool definition ) const;
  std::string generateRAIIHandleCommandDeclarations( std::pair<std::string, HandleData> const & handle, std::set<std::string> const & specialFunctions ) const;
  std::string generateRAIIHandleCommandEnhanced( std::string const &                       name,
                                                 CommandData const &                       commandData,
                                                 size_t                                    initialSkipCount,
                                                 std::vector<size_t> const &               returnParams,
                                                 std::map<size_t, VectorParamData> const & vectorParamIndices,
                                                 bool                                      definition,
                                                 CommandFlavourFlags                       flavourFlags = {} ) const;
  std::string generateRAIIHandleCommandFactory( std::string const &                       name,
                                                CommandData const &                       commandData,
                                                size_t                                    initialSkipCount,
                                                std::vector<size_t> const &               returnParams,
                                                std::map<size_t, VectorParamData> const & vectorParams,
                                                bool                                      definition,
                                                CommandFlavourFlags                       flavourFlags = {} ) const;
  std::string generateRAIIHandleCommandFactoryArgumentList( std::vector<ParamData> const & params,
                                                            std::set<size_t> const &       skippedParams,
                                                            bool                           definition,
                                                            bool                           singular ) const;
  std::string generateRAIIHandleCommandStandard( std::string const & name, CommandData const & commandData, size_t initialSkipCount, bool definition ) const;
  std::pair<std::string, std::string> generateRAIIHandleConstructor( std::pair<std::string, HandleData> const &         handle,
                                                                     std::map<std::string, CommandData>::const_iterator constructorIt,
                                                                     std::string const &                                enter,
                                                                     std::string const &                                leave ) const;
  std::string                         generateRAIIHandleConstructorByCall( std::pair<std::string, HandleData> const &         handle,
                                                                           std::map<std::string, CommandData>::const_iterator constructorIt,
                                                                           std::string const &                                enter,
                                                                           std::string const &                                leave,
                                                                           bool                                               isPlural,
                                                                           bool                                               forceSingular ) const;
  std::pair<std::string, std::string> generateRAIIHandleConstructor1Return2Vector( std::pair<std::string, HandleData> const &         handle,
                                                                                   std::map<std::string, CommandData>::const_iterator constructorIt,
                                                                                   std::string const &                                enter,
                                                                                   std::string const &                                leave,
                                                                                   size_t                                             returnParam,
                                                                                   std::map<size_t, VectorParamData> const & vectorParamIndices ) const;
  std::pair<std::string, std::string> generateRAIIHandleConstructors( std::pair<std::string, HandleData> const & handle ) const;
  std::string generateRAIIHandleConstructorArgument( ParamData const & param, bool definition, bool singular, bool takesOwnership ) const;
  std::string generateRAIIHandleConstructorArguments( std::pair<std::string, HandleData> const &         handle,
                                                      std::map<std::string, CommandData>::const_iterator constructorIt,
                                                      bool                                               singular,
                                                      bool                                               takesOwnership ) const;
  std::string generateRAIIHandleConstructorInitializationList( std::pair<std::string, HandleData> const &         handle,
                                                               std::map<std::string, CommandData>::const_iterator constructorIt,
                                                               std::map<std::string, CommandData>::const_iterator destructorIt,
                                                               bool                                               takesOwnership ) const;
  std::string generateRAIIHandleConstructorParamName( std::string const & type, std::map<std::string, CommandData>::const_iterator destructorIt ) const;
  std::pair<std::string, std::string> generateRAIIHandleConstructorResult( std::pair<std::string, HandleData> const &         handle,
                                                                           std::map<std::string, CommandData>::const_iterator constructorIt,
                                                                           std::string const &                                enter,
                                                                           std::string const &                                leave ) const;
  std::string                         generateRAIIHandleConstructorTakeOwnership( std::pair<std::string, HandleData> const & handle ) const;
  std::pair<std::string, std::string> generateRAIIHandleConstructorVoid( std::pair<std::string, HandleData> const &         handle,
                                                                         std::map<std::string, CommandData>::const_iterator constructorIt,
                                                                         std::string const &                                enter,
                                                                         std::string const &                                leave ) const;
  std::string generateRAIIHandleContext( std::pair<std::string, HandleData> const & handle, std::set<std::string> const & specialFunctions ) const;
  std::string generateRAIIHandleDestructorCallArguments( std::string const &                                handleType,
                                                         std::map<std::string, CommandData>::const_iterator destructorIt ) const;
  std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string>
              generateRAIIHandleDetails( std::pair<std::string, HandleData> const & handle ) const;
  std::string generateRAIIHandleForwardDeclarations( std::vector<RequireData> const & requireData, std::string const & title ) const;
  std::string generateRAIIHandles() const;
  std::string generateRAIIHandleSingularConstructorArguments( std::pair<std::string, HandleData> const & handle,
                                                              std::vector<ParamData> const &             params,
                                                              std::string const &                        argumentType,
                                                              std::string const &                        argumentName ) const;
  template <class Predicate, class Extraction>
  std::string generateReplacedExtensionsList( Predicate p, Extraction e ) const;
  std::string generateResultAssignment( CommandData const & commandData ) const;
  std::string generateResultCheck( CommandData const & commandData,
                                   std::string const & className,
                                   std::string const & classSeparator,
                                   std::string         commandName,
                                   bool                enumerating,
                                   bool                raii ) const;
  std::string generateResultExceptions() const;
  std::string generateReturnStatement( std::string const & commandName,
                                       CommandData const & commandData,
                                       std::string const & returnVariable,
                                       std::string const & returnType,
                                       std::string const & decoratedReturnType,
                                       std::string const & dataType,
                                       size_t              initialSkipCount,
                                       size_t              returnParam,
                                       CommandFlavourFlags flavourFlags,
                                       bool                enumerating,
                                       bool                raii ) const;
  std::string generateReturnType( std::vector<size_t> const &               returnParams,
                                  std::map<size_t, VectorParamData> const & vectorParams,
                                  CommandFlavourFlags                       flavourFlags,
                                  bool                                      raii,
                                  std::vector<std::string> const &          dataTypes ) const;
  std::string generateReturnVariable( CommandData const &                       commandData,
                                      std::vector<size_t> const &               returnParams,
                                      std::map<size_t, VectorParamData> const & vectorParams,
                                      CommandFlavourFlags                       flavourFlags ) const;
  std::string
    generateSizeCheck( std::vector<std::vector<MemberData>::const_iterator> const & arrayIts, std::string const & structName, bool mutualExclusiveLens ) const;
  std::string generateStaticAssertions() const;
  std::string generateStaticAssertions( std::vector<RequireData> const & requireData, std::string const & title, std::set<std::string> & listedStructs ) const;
  std::string generateStruct( std::pair<std::string, StructureData> const & structure, std::set<std::string> & listedStructs ) const;
  std::string generateStructCastAssignments( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateStructCompareOperators( std::pair<std::string, StructureData> const & structure ) const;
  std::string generateStructConstructors( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateStructConstructorsEnhanced( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateStructConstructorArgument( MemberData const & memberData, bool withDefault ) const;
  std::string generateStructCopyAssignment( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateStructCopyConstructor( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateStructHashStructure( std::pair<std::string, StructureData> const & structure, std::set<std::string> & listedStructs ) const;
  std::string generateStructHashStructures() const;
  std::string generateStructHashSum( std::string const & structName, std::vector<MemberData> const & members ) const;
  std::string generateStructs() const;
  std::string generateStructure( std::pair<std::string, StructureData> const & structure ) const;
  std::string generateStructExtendsStructs() const;
  std::string
    generateStructExtendsStructs( std::vector<RequireData> const & requireData, std::set<std::string> & listedStructs, std::string const & title ) const;
  std::string generateStructForwardDeclarations() const;
  std::string
    generateStructForwardDeclarations( std::vector<RequireData> const & requireData, std::string const & title, std::set<std::string> & listedStructs ) const;
  std::tuple<std::string, std::string, std::string, std::string> generateStructMembers( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateStructSetter( std::string const & structureName, std::vector<MemberData> const & memberData, size_t index ) const;
  std::string generateStructSubConstructor( std::pair<std::string, StructureData> const & structData ) const;
  std::string generateSuccessCheck( std::vector<std::string> const & successCodes ) const;
  std::string generateSuccessCode( std::string const & code ) const;
  std::string generateSuccessCodeList( std::vector<std::string> const & successCodes, bool enumerating ) const;
  std::string generateThrowResultException() const;
  std::string generateTypenameCheck( std::vector<size_t> const &               returnParams,
                                     std::map<size_t, VectorParamData> const & vectorParams,
                                     std::vector<size_t> const &               chainedReturnParams,
                                     bool                                      definition,
                                     std::vector<std::string> const &          dataTypes,
                                     CommandFlavourFlags                       flavourFlags ) const;
  std::string generateUnion( std::pair<std::string, StructureData> const & structure ) const;
  std::string generateUniqueHandle( std::pair<std::string, HandleData> const & handleData ) const;
  std::string generateUniqueHandle( std::vector<RequireData> const & requireData, std::string const & title, std::set<std::string> & listedHandles ) const;
  std::string generateUniqueHandles() const;
  std::string generateSharedHandle( std::pair<std::string, HandleData> const & handleData ) const;
  std::string generateSharedHandle( std::vector<RequireData> const & requireData, std::string const & title, std::set<std::string> & listedHandles ) const;
  std::string generateSharedHandleNoDestroy( std::pair<std::string, HandleData> const & handleData ) const;
  std::string generateSharedHandleNoDestroy( std::vector<RequireData> const & requireData, std::string const & title ) const;
  std::string generateSharedHandles() const;
  std::string generateSharedHandlesNoDestroy() const;
  std::string generateVectorSizeCheck( std::string const &                           name,
                                       CommandData const &                           commandData,
                                       size_t                                        initialSkipCount,
                                       std::map<size_t, std::vector<size_t>> const & countToVectorMap,
                                       std::set<size_t> const &                      skippedParams,
                                       bool                                          onlyThrows ) const;
  std::pair<std::string, std::string> getParentTypeAndName( std::pair<std::string, HandleData> const & handle ) const;
  std::string                         getPlatform( std::string const & title ) const;
  std::pair<std::string, std::string> getPoolTypeAndName( std::string const & type ) const;
  std::string                         getProtectFromPlatform( std::string const & platform ) const;
  std::string                         getProtectFromTitle( std::string const & title ) const;
  std::string                         getProtectFromTitles( std::set<std::string> const & titles ) const;
  std::string                         getProtectFromType( std::string const & type ) const;
  std::string                         getVectorSize( std::vector<ParamData> const &            params,
                                                     std::map<size_t, VectorParamData> const & vectorParamIndices,
                                                     size_t                                    returnParam,
                                                     std::string const &                       returnParamType,
                                                     std::set<size_t> const &                  templatedParams ) const;
  void                                handleRemoval( RemoveData const & removeData );
  bool                                handleRemovalCommand( std::string const & command, std::vector<RequireData> & requireData );
  void                                handleRemovals();
  bool                                handleRemovalType( std::string const & type, std::vector<RequireData> & requireData );
  bool                                hasArrayConstructor( HandleData const & handleData ) const;
  bool                                hasLen( MemberData const & md, std::vector<MemberData> const & members ) const;
  bool                                hasParentHandle( std::string const & handle, std::string const & parent ) const;
  bool isConstructorCandidate( std::pair<std::string, VulkanHppGenerator::CommandData> const & command, std::string const & handleType ) const;
  bool isConstructorCandidate( ParamData const & paramData, std::string const & handleType ) const;
  bool isDeviceCommand( CommandData const & commandData ) const;
  bool isEnumerated( std::string const & type ) const;
  bool isExtension( std::string const & name ) const;
  bool isFeature( std::string const & name ) const;
  bool isHandleType( std::string const & type ) const;
  bool isHandleTypeByStructure( std::string const & type ) const;
  bool isLenByStructMember( std::string const & name, std::vector<ParamData> const & params ) const;
  bool isLenByStructMember( std::string const & name, ParamData const & param ) const;
  bool isMultiSuccessCodeConstructor( std::vector<std::map<std::string, CommandData>::const_iterator> const & constructorIts ) const;
  bool isParam( std::string const & name, std::vector<ParamData> const & params ) const;
  bool isStructMember( std::string const & name, std::vector<MemberData> const & memberData ) const;
  bool isStructureChainAnchor( std::string const & type ) const;
  bool isStructureType( std::string const & type ) const;
  bool isSupported( std::set<std::string> const & requiredBy ) const;
  bool isSupportedExtension( std::string const & name ) const;
  bool isSupportedFeature( std::string const & name ) const;
  bool isTypeRequired( std::string const & type ) const;
  bool isTypeUsed( std::string const & type ) const;
  bool isUnsupportedExtension( std::string const & name ) const;
  bool isUnsupportedFeature( std::string const & name ) const;
  bool isVectorByStructure( std::string const & type ) const;
  void markExtendedStructs();
  bool needsStructureChainResize( std::map<size_t, VectorParamData> const & vectorParams, std::vector<size_t> const & chainedReturnParams ) const;
  std::pair<bool, std::map<size_t, std::vector<size_t>>> needsVectorSizeCheck( std::vector<ParamData> const &            params,
                                                                               std::map<size_t, VectorParamData> const & vectorParams,
                                                                               std::vector<size_t> const &               returnParams,
                                                                               std::set<size_t> const &                  singularParams,
                                                                               std::set<size_t> const &                  skippedParams ) const;
  void                                                   readCommand( tinyxml2::XMLElement const * element );
  std::pair<bool, ParamData>                             readCommandParam( tinyxml2::XMLElement const * element, std::vector<ParamData> const & params );
  std::pair<std::string, std::string>                    readCommandProto( tinyxml2::XMLElement const * element );
  void                                                   readCommands( tinyxml2::XMLElement const * element );
  std::string                                            readComment( tinyxml2::XMLElement const * element ) const;
  DeprecateData                                          readDeprecateData( tinyxml2::XMLElement const * element ) const;
  void                                                   readEnums( tinyxml2::XMLElement const * element );
  void                                                   readEnumsConstants( tinyxml2::XMLElement const * element );
  void readEnumsEnum( tinyxml2::XMLElement const * element, std::map<std::string, EnumData>::iterator enumIt );
  void readExtension( tinyxml2::XMLElement const * element );
  void readExtensionRequire( tinyxml2::XMLElement const * element, ExtensionData & extensionData, bool extensionSupported );
  void readExtensions( tinyxml2::XMLElement const * element );
  void readFeature( tinyxml2::XMLElement const * element );
  void readFeatureRequire( tinyxml2::XMLElement const * element, FeatureData & featureData, bool featureSupported );
  void readFormat( tinyxml2::XMLElement const * element );
  void readFormatComponent( tinyxml2::XMLElement const * element, FormatData & formatData );
  void readFormatPlane( tinyxml2::XMLElement const * element, FormatData & formatData );
  void readFormats( tinyxml2::XMLElement const * element );
  void readFormatSPIRVImageFormat( tinyxml2::XMLElement const * element, FormatData & formatData );
  std::pair<std::vector<std::string>, std::string> readModifiers( tinyxml2::XMLNode const * node ) const;
  std::string                                      readName( tinyxml2::XMLElement const * elements ) const;
  std::pair<NameData, TypeInfo>                    readNameAndType( tinyxml2::XMLElement const * elements );
  void                                             readPlatform( tinyxml2::XMLElement const * element );
  void                                             readPlatforms( tinyxml2::XMLElement const * element );
  void                                             readRegistry( tinyxml2::XMLElement const * element );
  RemoveData                                       readRemoveData( tinyxml2::XMLElement const * element );
  NameLine                                         readRequireCommand( tinyxml2::XMLElement const * element );
  void                                             readRequireEnum(
                                                tinyxml2::XMLElement const * element, std::string const & requiredBy, std::string const & platform, bool supported, RequireData & requireData );
  RequireFeature           readRequireFeature( tinyxml2::XMLElement const * element );
  NameLine                 readRequireType( tinyxml2::XMLElement const * element );
  void                     readSPIRVCapability( tinyxml2::XMLElement const * element );
  void                     readSPIRVCapabilityEnable( tinyxml2::XMLElement const * element, SpirVCapabilityData & capability );
  void                     readSPIRVCapabilities( tinyxml2::XMLElement const * element );
  void                     readSPIRVExtension( tinyxml2::XMLElement const * element );
  void                     readSPIRVExtensionEnable( tinyxml2::XMLElement const * element );
  void                     readSPIRVExtensions( tinyxml2::XMLElement const * element );
  void                     readStructMember( tinyxml2::XMLElement const * element, std::vector<MemberData> & members, bool isUnion );
  void                     readSync( tinyxml2::XMLElement const * element );
  void                     readSyncAccess( tinyxml2::XMLElement const * element, std::map<std::string, EnumData>::const_iterator accessFlagBits2It );
  void                     readSyncAccessEquivalent( tinyxml2::XMLElement const * element, std::map<std::string, EnumData>::const_iterator accessFlagBits2It );
  void                     readSyncAccessSupport( tinyxml2::XMLElement const * element );
  void                     readSyncPipeline( tinyxml2::XMLElement const * element );
  void                     readSyncStage( tinyxml2::XMLElement const * element, std::map<std::string, EnumData>::const_iterator stageFlagBits2It );
  void                     readSyncStageEquivalent( tinyxml2::XMLElement const * element, std::map<std::string, EnumData>::const_iterator stageFlagBits2It );
  void                     readSyncStageSupport( tinyxml2::XMLElement const * element );
  void                     readTag( tinyxml2::XMLElement const * element );
  void                     readTags( tinyxml2::XMLElement const * element );
  void                     readTypeBasetype( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  void                     readTypeBitmask( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  DefinesPartition         partitionDefines( std::map<std::string, DefineData> const & defines );
  void                     readTypeDefine( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  void                     readTypeEnum( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  void                     readTypeFuncpointer( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  void                     readTypeHandle( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  void                     readTypeInclude( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  void                     readTypeRequires( tinyxml2::XMLElement const * element, std::map<std::string, std::string> const & attributes );
  void                     readTypeStruct( tinyxml2::XMLElement const * element, bool isUnion, std::map<std::string, std::string> const & attributes );
  void                     readTypes( tinyxml2::XMLElement const * element );
  void                     readTypesType( tinyxml2::XMLElement const * element );
  TypeInfo                 readTypeInfo( tinyxml2::XMLElement const * element ) const;
  void                     readVideoCapabilities( tinyxml2::XMLElement const * element, VideoCodec & videoCodec );
  void                     readVideoCodec( tinyxml2::XMLElement const * element );
  void                     readVideoCodecs( tinyxml2::XMLElement const * element );
  void                     readVideoFormat( tinyxml2::XMLElement const * element, VideoCodec & videoCodec );
  void                     readVideoProfileMember( tinyxml2::XMLElement const * element, VideoCodec & videoCodec );
  void                     readVideoProfile( tinyxml2::XMLElement const * element, VideoCodec & videoCodec );
  void                     readVideoProfiles( tinyxml2::XMLElement const * element, VideoCodec & videoCodec );
  void                     readVideoFormatProperties( tinyxml2::XMLElement const * element, std::string const & videoCodec, VideoFormat & videoFormat );
  void                     readVideoRequireCapabilities( tinyxml2::XMLElement const * element, VideoCodec & videoCodec );
  void                     registerDeleter( std::string const & commandName, CommandData const & commandData );
  void                     rescheduleRAIIHandle( std::string &                              str,
                                                 std::pair<std::string, HandleData> const & handle,
                                                 std::set<std::string> &                    listedHandles,
                                                 std::set<std::string> const &              specialFunctions ) const;
  std::vector<std::string> selectCommandsByHandle( std::vector<RequireData> const & requireData,
                                                   std::set<std::string> const &    handleCommands,
                                                   std::set<std::string> &          listedCommands ) const;
  bool                     skipLeadingGrandParent( std::pair<std::string, HandleData> const & handle ) const;
  std::string              stripPluralS( std::string const & name ) const;
  std::string              toString( TypeCategory category );
  MemberData const &       vectorMemberByStructure( std::string const & structureType ) const;

private:
  std::string                         m_api;
  std::map<std::string, BaseTypeData> m_baseTypes;
  std::map<std::string, BitmaskData>  m_bitmasks;
  std::set<std::string>               m_commandQueues;
  std::map<std::string, CommandData>  m_commands;
  std::map<std::string, ConstantData> m_constants;
  std::map<std::string, DefineData>   m_defines;
  DefinesPartition                    m_definesPartition;  // partition defined macros into mutually-exclusive sets of callees, callers, and values
  std::map<std::string, std::vector<EnumExtendData>> m_enumExtends;
  std::map<std::string, EnumData>                    m_enums;
  std::vector<ExtensionData>                         m_extensions;
  std::map<std::string, ExternalTypeData>            m_externalTypes;
  std::vector<FeatureData>                           m_features;
  std::map<std::string, FormatData>                  m_formats;
  std::map<std::string, FuncPointerData>             m_funcPointers;
  std::map<std::string, HandleData>                  m_handles;
  std::map<std::string, IncludeData>                 m_includes;
  std::map<std::string, PlatformData>                m_platforms;
  std::set<std::string>                              m_RAIISpecialFunctions;
  std::map<std::string, SpirVCapabilityData>         m_spirVCapabilities;
  std::map<std::string, StructureData>               m_structs;
  std::vector<std::pair<std::string, NameLine>> m_structsAliases;  // temporary storage for aliases, as they might be listed before the actual struct is listed
  std::map<std::string, NameLine>               m_syncAccesses;
  std::map<std::string, NameLine>               m_syncStages;
  std::map<std::string, TagData>                m_tags;
  std::map<std::string, TypeData>               m_types;
  std::vector<ExtensionData>                    m_unsupportedExtensions;
  std::vector<FeatureData>                      m_unsupportedFeatures;
  std::string                                   m_version;
  std::vector<VideoCodec>                       m_videoCodecs;
  std::string                                   m_vulkanLicenseHeader;
};

template <typename T>
std::string VulkanHppGenerator::generateFormatTraitsSubCases( FormatData const &                                          formatData,
                                                              std::function<std::vector<T> const &( FormatData const & )> accessor,
                                                              std::string const &                                         subCaseName,
                                                              std::function<std::string( T const & subCaseData )>         generator,
                                                              std::string const &                                         defaultReturn ) const
{
  const std::string subCasesTemplate = R"(
        switch( ${subCaseName} )
        {
${subCases}
          default: VULKAN_HPP_ASSERT( false ); return ${defaultReturn};
        })";

  std::string subCases;
  for ( size_t i = 0; i < accessor( formatData ).size(); ++i )
  {
    subCases += "          case " + std::to_string( i ) + ": return " + generator( accessor( formatData )[i] ) + ";\n";
  }
  subCases.pop_back();

  return replaceWithMap( subCasesTemplate, { { "defaultReturn", defaultReturn }, { "subCaseName", subCaseName }, { "subCases", subCases } } );
}
