///////////////////////////////////////////////////////////////////////////////
///
/// \file EditorSearchProviders.hpp
/// Support for searching
///
/// Authors: Chris Peters
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

namespace Zero
{

class ResourceLibrary;
class PropertyInterface;


//--------------------------------------------------- ResourceSearchProvider ---
class ResourceSearchProvider : public SearchProvider
{
public:
  ResourceLibrary* mDefaultLibrary;
  ResourceLibrary* mResourceLibrary;
  bool mShowHidden;

  // If providing a default library, then that library should not contain hidden
  // content.  If the library contains hidden content, but needs to be shown,
  // then supply that library as the first arg.
  ResourceSearchProvider(ResourceLibrary* library, bool showHidden = false,
                         ResourceLibrary* defaultLib = nullptr);

  void RunCommand(SearchView* searchView, SearchViewResult& element) override;
  void Search(SearchData& search) override;

  void AttemptAddResource(SearchData& search, HashSet<String>& localTags, Resource* resource);

  Composite* CreatePreview(Composite* parent, SearchViewResult& element) override;
  String GetElementType(SearchViewResult& element) override;
};

//---------------------------------------------------- LibrarySearchProvider ---
class LibrarySearchProvider : public SearchProvider
{
public:
  bool mCanReturnResources;
  String mActiveLibrary;
  ResourceLibrary* mDefaultLibrary;
  ResourceSearchProvider mTargetResourceProvider;
  ContentSystem::ContentLibraryMapType& mLibraries;

  LibrarySearchProvider(bool canReturnResources, ResourceLibrary* defaultLibrary);

  bool OnMatch(SearchView* searchView, SearchViewResult& element) override;
  void RunCommand(SearchView* searchView, SearchViewResult& element) override;
  void Search(SearchData& search) override;
  String GetElementType(SearchViewResult& element) override;
};

//----------------------------------------------------- ObjectSearchProvider ---
// Search Provider for Objects in the Editor space
class ObjectSearchProvider : public SearchProvider
{
public:
  ObjectSearchProvider();

  void RunCommand(SearchView* searchView, SearchViewResult& element) override;

  /// Add an object the search results
  void AddObject(Cog& object, SearchData& search);
  void Search(SearchData& search) override;

  Composite* CreatePreview(Composite* parent, SearchViewResult& element) override;
  String GetElementType(SearchViewResult& element) override;
};

//-------------------------------------------------- ComponentSearchProvider ---
/// Search Provider for Components to add to compositions
/// using MetaComposition on MetaType
class ComponentSearchProvider : public SearchProvider
{
public:
  // Object to check for components to add.
  HandleOf<MetaComposition> mComposition;
  Handle mObject;

  ComponentSearchProvider();

  void Search(SearchData& search) override;

  String GetElementType(SearchViewResult& element) override;
  Composite* CreatePreview(Composite* parent, SearchViewResult& element) override;
};

//------------------------------------------------------------------ Helpers ---
SearchProvider* GetObjectSearchProvider();
SearchProvider* GetFactoryProvider(HandleParam object, HandleOf<MetaComposition>& composition);
SearchProvider* GetLibrarySearchProvider(bool canReturnResources = false, ResourceLibrary* defaultLibrary = nullptr);
SearchProvider* GetResourceSearchProvider(ResourceLibrary* resourceLibrary = NULL, bool showHidden = false);
void AddEditorProviders(SearchData& search);

}//namespace Zero
