// Copyright 2011 Google Inc. All Rights Reserved.
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

#include "eval_env.h"

string BindingEnv::LookupVariable(const string& var, bool for_rspfile) {
  map<string, string>::iterator i = bindings_.find(var);
  if (i != bindings_.end())
    return i->second;
  if (parent_)
    return parent_->LookupVariable(var, for_rspfile);
  return "";
}

void BindingEnv::AddBinding(const string& key, const string& val) {
  bindings_[key] = val;
}

string EvalString::Evaluate(Env* env, bool for_rspfile) const {
  string result;
  for (TokenList::const_iterator i = parsed_.begin(); i != parsed_.end(); ++i) {
    if (i->second == RAW)
      result.append(i->first);
    else
      result.append(env->LookupVariable(i->first, for_rspfile));
  }
  return result;
}

void EvalString::AddText(StringPiece text) {
  // Add it to the end of an existing RAW token if possible.
  if (!parsed_.empty() && parsed_.back().second == RAW) {
    parsed_.back().first.append(text.str_, text.len_);
  } else {
    parsed_.push_back(make_pair(text.AsString(), RAW));
  }
}
void EvalString::AddSpecial(StringPiece text) {
  parsed_.push_back(make_pair(text.AsString(), SPECIAL));
}

string EvalString::Serialize() const {
  string result;
  for (TokenList::const_iterator i = parsed_.begin();
       i != parsed_.end(); ++i) {
    result.append("[");
    if (i->second == SPECIAL)
      result.append("$");
    result.append(i->first);
    result.append("]");
  }
  return result;
}
