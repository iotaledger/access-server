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

package com.de.xain.emdac.main.ui;

import android.arch.lifecycle.ViewModelProvider;
import android.arch.lifecycle.ViewModelProviders;
import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.de.xain.emdac.CommunicationFragment;
import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.FragmentVehicleInfoListBinding;
import com.de.xain.emdac.di.Injectable;
import com.de.xain.emdac.main.model.VehicleInfo;
import com.de.xain.emdac.main.model.VehicleInfoListViewModel;
import com.de.xain.emdac.utils.ui.BackPressHandler;
import com.de.xain.emdac.utils.ui.ViewLifecycleFragment;

import java.util.ArrayList;
import java.util.List;

import javax.inject.Inject;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import timber.log.Timber;

public class VehicleInfoListFragment
        extends CommunicationFragment<VehicleInfoListViewModel>
        implements BackPressHandler {

    private final int ACTION_SAVE = 100;

    @Inject
    public ViewModelProvider.Factory mViewModelFactory;

    private FragmentVehicleInfoListBinding mBinding;
    private VehicleInfoAdapter mAdapter;
    private List<VehicleInfo> mVehicleInfoList;
    private List<VehicleInfo> mInitialVehicleInfoList;
    private boolean doubleBackToExitPressedOnce = false;
    private Toast mToast;
    private VehicleInfoListViewModel mViewModel;
    private CompositeDisposable mDisposable;

    @Override
    protected Class<VehicleInfoListViewModel> getViewModelClass() {
        return VehicleInfoListViewModel.class;
    }

    public static VehicleInfoListFragment newInstance() {
        return new VehicleInfoListFragment();
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_vehicle_info_list, container, false);
        mBinding.recyclerView.setLayoutManager(new LinearLayoutManager(getActivity()));
        return mBinding.getRoot();
    }

    @Override
    public void onStart() {
        super.onStart();
        mViewModel.requestVehicleInfoList();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders
                .of(this, mViewModelFactory)
                .get(VehicleInfoListViewModel.class);
        getLifecycle().addObserver(mViewModel);
        mVehicleInfoList = new ArrayList<>();
        mAdapter = new VehicleInfoAdapter(mVehicleInfoList);
        mBinding.recyclerView.setAdapter(mAdapter);
        mBinding.swipeRefreshLayout.setOnRefreshListener(mViewModel::requestVehicleInfoList);
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        MenuItem save = menu.add(Menu.NONE, ACTION_SAVE, Menu.NONE, R.string.action_save);
        save.setIcon(R.drawable.ic_save_white);
        save.setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM | MenuItem.SHOW_AS_ACTION_WITH_TEXT);
        super.onCreateOptionsMenu(menu, inflater);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case ACTION_SAVE:
                onSaveAction();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    protected void bindViewModel() {
        super.bindViewModel();
        mDisposable = new CompositeDisposable();

        // observe for vehicle info lists
        mDisposable.add(mViewModel
                .getObservableVehicleInfoList()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(vehicleInfoList -> {
                    mVehicleInfoList.clear();
                    mVehicleInfoList.addAll(vehicleInfoList);
                    mInitialVehicleInfoList = new ArrayList<>();
                    for (VehicleInfo vehicleInfo : vehicleInfoList)
                        mInitialVehicleInfoList.add(vehicleInfo.clone());
                    mAdapter.notifyDataSetChanged();
                }));

        // subscribe for when to show refresh
        mDisposable.add(mViewModel
                .getShowRefresh()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::showRefresh,
                        // on error
                        Timber::e));
    }

    @Override
    protected void unbindViewModel() {
        super.unbindViewModel();
        if (mDisposable != null && !mDisposable.isDisposed()) {
            mDisposable.dispose();
        }
    }

    private void showRefresh(boolean flag) {
        mBinding.swipeRefreshLayout.setRefreshing(flag);
    }

    /**
     * Called when user taps on save action in action bar.
     */
    private void onSaveAction() {
        showProgress(getString(R.string.msg_saving), null);
        mViewModel.saveVehicleInfoList(mVehicleInfoList);
    }

    /**
     * On back button press check if currently displayed list is different
     * from its initial state. If it is different, display toast to inform user
     * that those changes will be discarded. Double press on back button
     * exits fragment.
     */
    @Override
    public void handleOnBackPress() {
        if (isListChanged()) {
            if (doubleBackToExitPressedOnce) {
                finish();
            } else {
                doubleBackToExitPressedOnce = true;
                mToast = Toast.makeText(getActivity(),
                        R.string.msg_press_back_again_to_discard,
                        Toast.LENGTH_SHORT);
                mToast.show();
                new Handler().postDelayed(() -> doubleBackToExitPressedOnce = false, 2000);
            }
        } else {
            finish();
        }
    }

    /**
     * Check if items in list have changed relative to initial list's state.
     *
     * @return true if list has changed, false otherwise
     */
    private boolean isListChanged() {
        if (mInitialVehicleInfoList == null || mVehicleInfoList == null) return false;
        if (mInitialVehicleInfoList.size() != mVehicleInfoList.size()) return true;
        for (int i = 0, size = mVehicleInfoList.size(); i < size; i++)
            if (mInitialVehicleInfoList.get(i).isSelected() != mVehicleInfoList.get(i).isSelected())
                return true;
        return false;
    }

    @Override
    protected void finish() {
        if (mToast != null) {
            mToast.cancel();
        }
        super.finish();
    }
}
