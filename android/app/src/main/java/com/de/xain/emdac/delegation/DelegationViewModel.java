/*
 *  This file is part of the Frost distribution
 *  (https://github.com/xainag/frost)
 *
 *  Copyright (c) 2019 XAIN AG.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package com.de.xain.emdac.delegation;

import android.databinding.Bindable;
import android.support.annotation.Nullable;
import android.util.Pair;

import com.de.xain.emdac.BR;
import com.de.xain.emdac.CommunicationViewModel;
import com.de.xain.emdac.R;
import com.de.xain.emdac.api.Communicator;
import com.de.xain.emdac.api.PSService;
import com.de.xain.emdac.api.model.CommunicationMessage;
import com.de.xain.emdac.api.model.TCPResponse;
import com.de.xain.emdac.api.model.policy.Policy;
import com.de.xain.emdac.api.model.policy.PolicyAttribute;
import com.de.xain.emdac.api.model.policy.PolicyAttributeList;
import com.de.xain.emdac.api.model.policy.PolicyAttributeSingle;
import com.de.xain.emdac.api.model.policy.PolicyObject;
import com.de.xain.emdac.api.model.policy_server.PSDelegatePolicyRequest;
import com.de.xain.emdac.api.model.policy_server.PSEmptyResponse;
import com.de.xain.emdac.models.User;
import com.de.xain.emdac.user.UserManager;
import com.de.xain.emdac.utils.JsonUtils;
import com.de.xain.emdac.utils.Optional;
import com.de.xain.emdac.utils.ResourceProvider;
import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonSyntaxException;
import com.google.gson.reflect.TypeToken;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import javax.inject.Inject;

import io.reactivex.Observable;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.schedulers.Schedulers;
import io.reactivex.subjects.BehaviorSubject;
import timber.log.Timber;

import static com.de.xain.emdac.api.model.policy.PolicyAttributeList.Operation.AND;
import static com.de.xain.emdac.api.model.policy.PolicyAttributeList.Operation.EQUAL;
import static com.de.xain.emdac.api.model.policy.PolicyAttributeList.Operation.OR;
import static com.de.xain.emdac.utils.Constants.TOKEN_SCALE_FACTOR;

@SuppressWarnings({"FieldCanBeLocal", "WeakerAccess"})
public class DelegationViewModel extends CommunicationViewModel {

    public static final float[] COST_VALUES = new float[]{0f, 50f / TOKEN_SCALE_FACTOR, 100f / TOKEN_SCALE_FACTOR, 150f / TOKEN_SCALE_FACTOR};

    //    public final UserPublic[] ALL_USERS;
    public final DelegationAction[] ALL_ACTIONS;

    private final BehaviorSubject<Optional<List<UserPublic>>> mAllUsers = BehaviorSubject.createDefault(new Optional<>(null));

    private final Gson mGson;
    private final String COMMAND_ADD_POLICY = "add_policy";
    private final BehaviorSubject<Set<UserPublic>> mSelectedUsers = BehaviorSubject.createDefault(new HashSet<>());
    private final BehaviorSubject<Optional<Rule>> mRuleGoC = BehaviorSubject.createDefault(new Optional<>(null));
    private final BehaviorSubject<Optional<Rule>> mRuleDoC = BehaviorSubject.createDefault(new Optional<>(null));
    private final BehaviorSubject<List<DelegationAction>> mDelegationActionList = BehaviorSubject.createDefault(new ArrayList<>());
    private final BehaviorSubject<Optional<Integer>> mMaxNumOfExecutions = BehaviorSubject.createDefault(new Optional<>(null));
    private final BehaviorSubject<List<Rule>> mRuleList = BehaviorSubject.createDefault(new ArrayList<>());
    private final PSService mPSService;
    private final UserManager mUserManager;

    @Nullable
    private CompositeDisposable mRequestsDisposable;

    @Nullable
    private List<Observable<PSEmptyResponse>> mRequests;

    private int mSelectedCostIndex;

    @Inject
    public DelegationViewModel(Communicator communicator, Gson gson, PSService psService, ResourceProvider resourceProvider, UserManager userManager) {
        super(communicator, resourceProvider);
        mGson = gson;
        mPSService = psService;
        mUserManager = userManager;
        String[] actionNames = mResourceProvider.getResources().getStringArray(R.array.default_commands);
        String[] actionValues = mResourceProvider.getResources().getStringArray(R.array.default_commands_values);

        List<DelegationAction> actions = new ArrayList<>();
        for (int i = 0; i < actionNames.length; ++i)
            actions.add(new DelegationAction(actionNames[i], actionValues[i]));
        ALL_ACTIONS = actions.toArray(new DelegationAction[0]);

        setSelectedCostIndex(0);
    }

    public BehaviorSubject<Optional<List<UserPublic>>> getAllUsers() {
        return mAllUsers;
    }

    public void requestAllUsers() {
        sendTCPMessage(CommunicationMessage.makeGetAllUsersRequest(), mResourceProvider.getString(R.string.msg_requesting_users));
    }

    public void updateRule(Rule rule) {
        List<Rule> ruleList = mRuleList.getValue();
        int index = ruleList.indexOf(rule);
        if (index != -1)
            ruleList.set(index, rule);
        mRuleList.onNext(ruleList);
    }

    public void addRule(Rule rule) {
        List<Rule> ruleList = mRuleList.getValue();
        ruleList.add(rule);
        mRuleList.onNext(ruleList);
    }

    public void removeRule(Rule rule) {
        List<Rule> ruleList = mRuleList.getValue();
        ruleList.remove(rule);
        mRuleList.onNext(ruleList);
    }

    public Observable<List<Rule>> getObservableRuleList() {
        return mRuleList;
    }

    public Observable<List<DelegationAction>> getObservableDelegationActionList() {
        return mDelegationActionList;
    }

    public List<DelegationAction> getDelegationActionList() {
        return mDelegationActionList.getValue();
    }

    public void setDelegationList(List<DelegationAction> delegationActionList) {
        mDelegationActionList.onNext(delegationActionList);
    }

    public Observable<Optional<Integer>> getObservableMaxNumOfExecutions() {
        return mMaxNumOfExecutions;
    }

    public void setMaxNumOfExecutions(@Nullable Integer value) {
        mMaxNumOfExecutions.onNext(new Optional<>(value));
    }

    @Bindable
    public int getSelectedCostIndex() {
        return mSelectedCostIndex;
    }

    public void setSelectedCostIndex(int selectedCostIndex) {
        mSelectedCostIndex = selectedCostIndex;
        notifyPropertyChanged(BR.selectedCostIndex);
    }

    public Observable<Set<UserPublic>> getObservableSelectedUsers() {
        return mSelectedUsers;
    }

    public Set<UserPublic> getSelectedUsers() {
        return mSelectedUsers.getValue();
    }

    public void setSelectedUsers(Set<UserPublic> selectedUsers) {
        mSelectedUsers.onNext(selectedUsers);
    }

    public Observable<Optional<Rule>> getObservableRuleGoC() {
        return mRuleGoC;
    }

    public Observable<Optional<Rule>> getObservableRuleDoC() {
        return mRuleDoC;
    }

    public Rule getRuleGoC() {
        if (mRuleGoC.getValue().isEmpty())
            return null;
        else
            return mRuleGoC.getValue().get();
    }

    public void setRuleGoC(Rule rule) {
        mRuleGoC.onNext(new Optional<>(rule));
    }

    public Rule getRuleDoC() {
        if (mRuleDoC.getValue().isEmpty())
            return null;
        else
            return mRuleDoC.getValue().get();
    }

    public void setRuleDoC(Rule rule) {
        mRuleDoC.onNext(new Optional<>(rule));
    }

    public void delegate() {
        if (mSelectedUsers.getValue().size() == 0) {
            mShowDialogMessage.onNext(mResourceProvider.getString(R.string.error_msg_no_users_selected));
            return;
        }

        if (mDelegationActionList.getValue().size() == 0) {
            mShowDialogMessage.onNext(mResourceProvider.getString(R.string.error_msg_no_actions_selected));
            return;
        }

        if (mUserManager.getUser() == null) {
            mShowDialogMessage.onNext(mResourceProvider.getString(R.string.error_msg_user_must_be_logged_in));
            return;
        }

        mShowLoading.onNext(new Pair<>(true, mResourceProvider.getString(R.string.msg_delegating)));

        mRequests = new ArrayList<>();


        for (DelegationAction action : getDelegationActionList()) {
            PSDelegatePolicyRequest request = new PSDelegatePolicyRequest(
                    mUserManager.getUser().getUsername(),
                    mUserManager.getUser().getDeviceId(),
                    createPolicy(false, action));
            mRequests.add(mPSService
                    .delegatePolicy(request)
                    .subscribeOn(Schedulers.io()));
        }

        if (mRequestsDisposable != null) {
            mRequestsDisposable.dispose();
        } else {
            mRequestsDisposable = new CompositeDisposable();
        }
        Observable<Boolean> combined = Observable.zip(mRequests, responses -> {
            boolean isSuccess = true;
            for (Object object : responses) {
                if (!(object instanceof PSEmptyResponse)) return false;
                PSEmptyResponse response = (PSEmptyResponse) object;
                isSuccess = !response.isError() && isSuccess;
                Timber.d(response.toString());
            }
            return isSuccess;
        });

        mRequestsDisposable.add(combined
                .subscribeOn(Schedulers.io())
                .subscribe(
                        // on next
                        isSuccessful -> {
                            if (mRequestsDisposable != null)
                                mRequestsDisposable.dispose();
                            mRequestsDisposable = null;
                            mShowLoading.onNext(new Pair<>(false, null));
                            if (isSuccessful) {
                                mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_delegating_success));
                            } else {
                                mShowDialogMessage.onNext(mResourceProvider.getString(R.string.error_msg_delegation_failed));
                            }
                        },
                        // on error
                        throwable -> {
                            if (mRequestsDisposable != null)
                                mRequestsDisposable.dispose();
                            mRequestsDisposable = null;
                            mShowLoading.onNext(new Pair<>(false, null));
                            mShowDialogMessage.onNext(throwable.getLocalizedMessage());
                        }
                ));
    }

    public Policy createPolicy(boolean hide, DelegationAction action) {
        PolicyObject policyObject = new PolicyObject();
        User user = mUserManager.getUser();

        if (user == null) return null;

        PolicyAttribute users = createUsersPolicyAttribute(hide);
//        PolicyAttribute device = createObjectPolicyAttribute(user.getDeviceId());

        // set policy GoC
        PolicyAttributeList policyGoC;
        policyGoC = new PolicyAttributeList();
        policyGoC.setOperation(AND);

        PolicyAttribute combinedRules = combineRules(mRuleList.getValue());
        if (combinedRules != null)
            policyGoC.addPolicyAttribute(combinedRules);

//        if (!mRuleGoC.getValue().isEmpty())
//            policyGoC.addPolicyAttribute(mRuleGoC.getValue().get().toPolicyAttribute());
        if (action != null)
            policyGoC.addPolicyAttribute(createActionPolicyAttribute(action));
        if (users != null)
            policyGoC.addPolicyAttribute(users);
        if (!mMaxNumOfExecutions.getValue().isEmpty())
            policyGoC.addPolicyAttribute(new ExecuteNumberRule(mMaxNumOfExecutions.getValue().get()).toPolicyAttribute());
//        if (device != null)
//            policyGoC.addPolicyAttribute(device);

        policyObject.setPolicyGoc(policyGoC);

        // set policy DoC
        PolicyAttribute policyDoC;
        if (!mRuleDoC.getValue().isEmpty()) {
            policyDoC = mRuleDoC.getValue().get().toPolicyAttribute();
        } else {
            policyDoC = new PolicyAttributeSingle("boolean", "false");
        }
        policyObject.setPolicyDoc(policyDoC);

        Policy policy = new Policy();
        policy.setHashFunction("sha-256");
        policy.setCost(COST_VALUES[mSelectedCostIndex]);
        policy.setPolicyObject(policyObject);
        if (hide)
            policy.setPolicyId("**********");
        return policy;
    }

    @Nullable
    private PolicyAttribute combineRules(List<Rule> ruleList) {
        if (ruleList.isEmpty()) return null;

        PolicyAttributeList policyAttributeList = new PolicyAttributeList();

        for (Rule rule : ruleList) {
            policyAttributeList.addPolicyAttribute(rule.toPolicyAttribute());
        }
        policyAttributeList.setOperation(AND);
        return policyAttributeList;
    }

    /**
     * Creates policy attributes for every user.
     *
     * @param hide if true, public_id of every user will be replaced with **********
     * @return policy attribute
     */
    @Nullable
    private PolicyAttribute createUsersPolicyAttribute(boolean hide) {
        if (mSelectedUsers.getValue().size() == 0) return null;

        if (mSelectedUsers.getValue().size() == 1) {
            UserPublic user = mSelectedUsers.getValue().iterator().next();
            PolicyAttributeList singleUser = new PolicyAttributeList();
            singleUser.setOperation(EQUAL);
            singleUser.addPolicyAttribute(new PolicyAttributeSingle("public_id", hide ? "**********" : user.getPublicIdHex()));
            singleUser.addPolicyAttribute(new PolicyAttributeSingle("request.subject.type", "request.subject.value"));
            return singleUser;
        } else {
            PolicyAttributeList finalList = new PolicyAttributeList();
            finalList.setOperation(OR);
            for (UserPublic user : mSelectedUsers.getValue()) {
                PolicyAttributeList singleUser = new PolicyAttributeList();
                singleUser.setOperation(EQUAL);
                singleUser.addPolicyAttribute(new PolicyAttributeSingle("public_id", hide ? "**********" : user.getPublicIdHex()));
                singleUser.addPolicyAttribute(new PolicyAttributeSingle("request.subject.type", "request.subject.value"));
                finalList.addPolicyAttribute(singleUser);
            }
            return finalList;
        }
    }

    private PolicyAttribute createObjectPolicyAttribute(final String deviceId) {
        PolicyAttributeList attributeList = new PolicyAttributeList();
        attributeList.setOperation(EQUAL);
        attributeList.addPolicyAttribute(new PolicyAttributeSingle("device_id", deviceId));
        attributeList.addPolicyAttribute(new PolicyAttributeSingle("request.object.type", "request.object.value"));
        return attributeList;
    }

    private PolicyAttribute createActionPolicyAttribute(DelegationAction action) {
        PolicyAttributeList finalList = new PolicyAttributeList();
        finalList.setOperation(EQUAL);
        finalList.addPolicyAttribute(new PolicyAttributeSingle("action", action.getValue()));
        finalList.addPolicyAttribute(new PolicyAttributeSingle("action", action.getValue()));
        return finalList;
    }

    @Override
    protected void handleTCPResponse(String sentMessage, String response) {
        super.handleTCPResponse(sentMessage, response);

        String cmd = CommunicationMessage.getCmdFromMessage(sentMessage);
        if (cmd == null) return;

        JsonElement jsonElement = JsonUtils.extractJsonElement(response);
        if (jsonElement == null) return;

        if (cmd.equals(CommunicationMessage.GET_ALL_USERS)) {
            try {
                TCPResponse<List<UserPublic>> tcpResponse = mGson.fromJson(jsonElement,
                        new TypeToken<TCPResponse<List<UserPublic>>>() {
                        }.getType());
                if (tcpResponse.isSuccessful()) {
                    if (tcpResponse.getData() != null) {
                        mAllUsers.onNext(new Optional<>(tcpResponse.getData()));
                    } else {
                        mShowDialogMessage.onNext(mResourceProvider.getString(R.string.something_wrong_happened));
                    }
                } else {
                    String message = tcpResponse.getMessage();
                    if (message == null)
                        message = mResourceProvider.getString(R.string.something_wrong_happened);
                    mShowDialogMessage.onNext(message);
                }
            } catch (JsonSyntaxException ignored) {
                mShowDialogMessage.onNext(mResourceProvider.getString(R.string.something_wrong_happened));
            }
        }

    }

    enum PolicyObjectType {
        GOC,
        DOC
    }
}
