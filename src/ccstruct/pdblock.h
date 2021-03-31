/**********************************************************************
 * File:        pdblock.h  (Formerly pdblk.h)
 * Description: Page block class definition.
 * Author:      Ray Smith
 *
 * (C) Copyright 1991, Hewlett-Packard Ltd.
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 ** http://www.apache.org/licenses/LICENSE-2.0
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 *
 **********************************************************************/

#ifndef PDBLOCK_H
#define PDBLOCK_H

#include "clst.h"
#include "polyblk.h"

struct Pix;

namespace tesseract {

class PDBLK; // forward decl

CLISTIZEH(PDBLK)
/// page block
class PDBLK {
  friend class BLOCK_RECT_IT; ///< block iterator
  friend class BLOCK;         ///< Page Block

public:
  /// empty constructor
  PDBLK() {
    hand_poly = nullptr;
    index_ = 0;
  }
  /// simple constructor
  PDBLK(int16_t xmin, ///< bottom left
        int16_t ymin,
        int16_t xmax, ///< top right
        int16_t ymax);

  /// set vertex lists
  ///@param left list of left vertices
  ///@param right list of right vertices
  void set_sides(ICOORDELT_LIST *left, ICOORDELT_LIST *right);

  /// destructor
  ~PDBLK() {
    delete hand_poly;
  }

  POLY_BLOCK *poly_block() const {
    return hand_poly;
  }
  /// set the poly block
  void set_poly_block(POLY_BLOCK *blk) {
    hand_poly = blk;
  }
  /// get box
  void bounding_box(ICOORD &bottom_left,       // bottom left
                    ICOORD &top_right) const { // topright
    bottom_left = box.botleft();
    top_right = box.topright();
  }
  /// get real box
  const TBOX &bounding_box() const {
    return box;
  }

  int index() const {
    return index_;
  }
  void set_index(int value) {
    index_ = value;
  }

  /// is pt inside block
  bool contains(ICOORD pt);

  /// reposition block
  void move(const ICOORD vec); // by vector

  // Returns a binary Pix mask with a 1 pixel for every pixel within the
  // block. Rotates the coordinate system by rerotation prior to rendering.
  // If not nullptr, mask_box is filled with the position box of the returned
  // mask image.
  Image render_mask(const FCOORD &rerotation, TBOX *mask_box);

#ifndef GRAPHICS_DISABLED
  /// draw histogram
  ///@param window window to draw in
  ///@param serial serial number
  ///@param colour colour to draw in
  void plot(ScrollView *window, int32_t serial, ScrollView::Color colour);
#endif // !GRAPHICS_DISABLED

  /// assignment
  ///@param source from this
  PDBLK &operator=(const PDBLK &source);

protected:
  POLY_BLOCK *hand_poly;    ///< weird as well
  ICOORDELT_LIST leftside;  ///< left side vertices
  ICOORDELT_LIST rightside; ///< right side vertices
  TBOX box;                 ///< bounding box
  int index_;               ///< Serial number of this block.
};

class BLOCK_RECT_IT // rectangle iterator
{
public:
  /// constructor
  ///@param blkptr block to iterate
  BLOCK_RECT_IT(PDBLK *blkptr);

  /// start (new) block
  void set_to_block(PDBLK *blkptr); // block to iterate

  /// start iteration
  void start_block();

  /// next rectangle
  void forward();

  /// test end
  bool cycled_rects() {
    return left_it.cycled_list() && right_it.cycled_list();
  }

  /// current rectangle
  ///@param bleft bottom left
  ///@param tright top right
  void bounding_box(ICOORD &bleft, ICOORD &tright) {
    // bottom left
    bleft = ICOORD(left_it.data()->x(), ymin);
    // top right
    tright = ICOORD(right_it.data()->x(), ymax);
  }

private:
  int16_t ymin = 0;       ///< bottom of rectangle
  int16_t ymax = 0;       ///< top of rectangle
  PDBLK *block = nullptr; ///< block to iterate
  ICOORDELT_IT left_it;   ///< boundary iterators
  ICOORDELT_IT right_it;
};

/// rectangle iterator
class BLOCK_LINE_IT {
public:
  /// constructor
  ///@param blkptr from block
  BLOCK_LINE_IT(PDBLK *blkptr) : rect_it(blkptr) {
    block = blkptr; // remember block
  }

  /// start (new) block
  ///@param blkptr block to start
  void set_to_block(PDBLK *blkptr) {
    block = blkptr; // remember block
                    // set iterator
    rect_it.set_to_block(blkptr);
  }

  /// get a line
  ///@param y line to get
  ///@param xext output extent
  int16_t get_line(int16_t y, int16_t &xext);

private:
  PDBLK *block;          ///< block to iterate
  BLOCK_RECT_IT rect_it; ///< rectangle iterator
};

} // namespace tesseract

#endif
