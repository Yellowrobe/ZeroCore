///////////////////////////////////////////////////////////////////////////////
///
/// \file SearchView.hpp
///  Search View provides generic search functionality.
///
/// Authors: Chris Peters
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

namespace Zero
{

// Forward Declarations
struct SearchViewResult;
struct SearchData;
class ScrollArea;
class SearchProvider;
class SearchViewElement;
class TextBox;
class KeyboardEvent;
class SearchView;
class TagChainTextBox;

namespace Events
{
  DeclareEvent(SearchCanceled);
  DeclareEvent(SearchPreview);
  DeclareEvent(SearchCompleted);
}

/// Event Sent by the search view..
class SearchViewEvent : public Event
{
public:
  ZilchDeclareType(SearchViewEvent, TypeCopyMode::ReferenceType);

  SearchView* View;
  SearchViewResult* Element;
};

// Boost priority to make differing result types appear above other result types.
// The higher the range boost value, the higher the sort priority order.
namespace SearchViewResultPriority
{

enum PriorityRange
{
  CommandBegin = 1000,
  LibraryBegin = 2000,
  TagBegin = 3000
};

}

/// Possible search match element
struct SearchViewResult
{
public:
  SearchViewResult()
    : Data(nullptr)
  {

  }
  // Provider that added this result
  SearchProvider* Interface;
  // Name match Priority
  int Priority;
  // Match User Data
  void* Data;
  // Handle to object
  Handle ObjectHandle;
  // Name of the Match
  String Name;
  // Whether or not the result is considered valid (displayed grayed out if not)
  Status mStatus;
};

class SearchProviderFilter
{
public:
  virtual bool FilterResult(SearchViewResult& result) = 0;
};

template <typename T, bool (T::*Func)(SearchViewResult& result)>
class SearchProviderFilterMethod : public SearchProviderFilter
{
public:
  SearchProviderFilterMethod(T* instance)
    : mInstance(instance)
  {

  }

  bool FilterResult(SearchViewResult& result) override
  {
    return (mInstance->*Func)(result);
  }

  T* mInstance;
};

/// Interface to providing search elements and execute matching.
class SearchProvider
{
public:
  SearchProvider(StringParam providerType = "TypeInvalid") : mFilter(nullptr), mProviderType(providerType) {}

  //Virtual Destructor for cleanup 
  virtual ~SearchProvider(){}
  //Collect Search Results
  virtual void Search(SearchData& search){}
  //Get an element's display type
  virtual String GetElementType(SearchViewResult& element){return String();}
  //Get an element's display name
  virtual String GetElementName(SearchViewResult& element) { return element.Name; }
  //Get a small icon to display
  virtual String GetIcon(SearchViewResult& element){return String();}
  //Used for tags element does not end search
  virtual bool OnMatch(SearchView* searchView, SearchViewResult& element){return true;}
  //Run auto completed command
  virtual void RunCommand(SearchView* searchView, SearchViewResult& element){};
  //Create a preview widget
  virtual Composite* CreatePreview(Composite* parent, SearchViewResult& element){return NULL;}

  virtual bool FilterResult(SearchViewResult& result);

  //Get an element's display name with its provider's display type
  String GetElementNameAndSearchType(SearchViewResult& element)
  {
    ReturnIf(mProviderType == "TypeInvalid", mProviderType, "Provider type must be specified.");
    return BuildString(GetElementName(element), "(", mProviderType, ")");
  }

  template <typename T, bool (T::*Func)(SearchViewResult& result)>
  void SetCallbackFilter(T* instance)
  {
    mFilter = new SearchProviderFilterMethod<T, Func>(instance);
  }

  // Helps differentiate name-collisions on list items.
  String mProviderType;

  // This allows for an extra step that filters what a search provider would normally return.
  SearchProviderFilter* mFilter;
};

//--------------------------------------------------------------- Search Data
/// All data needed to preform a search 
struct SearchData
{
  ~SearchData();

  void Search();

  // Output Data
  void AddAvailableTagsToResults();
  void Sort();
  void ClearSearchProviders();

  // Search Providers
  Array<SearchProvider*> SearchProviders;

  // String being searched
  String SearchString;
  // Tags active for this search
  TagList ActiveTags;
  // Meta types active for this search
  Array<BoundType*> ActiveMeta;

  // Tags found in this search
  TagList AvailableTags;
  // Search results
  Array<SearchViewResult> Results;
};

//------------------------------------------------------- Search View Element
class SearchViewElement : public Composite
{
public:
  ZilchDeclareType(SearchViewElement, TypeCopyMode::ReferenceType);

  SearchViewElement(Composite* parent);
  void OnMouseUp(MouseEvent* event);
  void OnMouseMove(MouseEvent* event);
  void Setup(SearchView* view, uint index, bool selected, SearchViewResult& element);
  void UpdateTransform() override;

  SearchView* mView;
  Text* mName;
  Text* mType;
  Element* mBackground;
  uint mIndex;
};

/// Search view searches for results with given providers.
class SearchView : public Composite
{
public:
  ZilchDeclareType(SearchView, TypeCopyMode::ReferenceType);
  
  SearchView(Composite* parent);
  ~SearchView();

  /// Start a search with a string
  void Search(StringParam text);
  void OnDestroy() override;

  //Add a tag to the search and clear text.
  void AddTag(StringParam tag, bool removeable = true);
  void AddHiddenTag(StringParam tag);

  void AddMetaType(BoundType* meta);

  bool TakeFocusOverride() override;

  // Active Search Data.
  SearchData* mSearch;

private:
  void UpdateTransform() override;
  void PositionToolTip();

  void OnSearchDataModified(Event* e);
  void OnSearchKeyPreview(KeyboardEvent* e);
  void Canceled();
  void Selected();
  void BuildResults();
  void MoveSelection(int index);
  void SetSelection(int index);
  void OnEnter(ObjectEvent* event);
  void OnKeyPressed(KeyboardEvent* event);
  void OnScrollUpdated(Event* e);

  //Active Elements
  Array<SearchViewElement*> mElements;
  //Current Preview Object
  HandleOf<ToolTip> mToolTip;
  void* mPreviewData;
  uint mSelectedIndex;
  // Searching Text Box
  TagChainTextBox* mSearchBar;
  // Results area
  ScrollArea* mArea;
  Element* mBackground;

  // Whether or not we sent out an event saying we completed or failed.
  bool mEventTerminated;

  friend class SearchViewElement;
};

/// Returns true if there are any tags that would reject this entry.
bool CheckTags(HashSet<String>& testTags, HashSet<String>& tags);
bool CheckTags(HashSet<String>& testTags, BoundType* type);

/// Check tags and if passes add all other tags.
bool CheckAndAddTags(SearchData& search, HashSet<String>& tags);
bool CheckAndAddTags(SearchData& search, BoundType* type);

/// Logic for simple filter if the tag is present or there
/// are no active tags add the tag and return true.
bool CheckAndAddSingleTag(SearchData& search, StringParam tag);

//Create a simple text box preview.
Composite* CreateTextPreview(Composite* parent, StringParam text);

}
